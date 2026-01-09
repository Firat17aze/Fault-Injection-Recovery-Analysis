#!/usr/bin/env python3
"""
FIRA - Serial Data Logger & Analyzer
=====================================

This script captures Serial output from the Arduino running FIRA,
logs it to a CSV file, and generates research statistics.

Usage:
    python3 fira_logger.py /dev/cu.usbmodem* output.csv

Requirements:
    pip install pyserial pandas matplotlib
"""

import serial
import serial.tools.list_ports
import sys
import time
import csv
import re
from datetime import datetime
from collections import defaultdict

# Configuration
BAUD_RATE = 115200
LOG_INTERVAL = 1  # seconds


def find_arduino_port():
    """Auto-detect Arduino serial port."""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if 'Arduino' in port.description or 'usbmodem' in port.device or 'usbserial' in port.device:
            return port.device
    return None


def parse_heartbeat(line):
    """Extract data from heartbeat line."""
    # Pattern: System Running: 123  [Uptime: 45s, Faults: 6]
    pattern = r"System Running:\s*(\d+).*Uptime:\s*(\d+)s.*Faults:\s*(\d+)"
    match = re.search(pattern, line)
    if match:
        return {
            'counter': int(match.group(1)),
            'uptime': int(match.group(2)),
            'faults': int(match.group(3))
        }
    return None


def parse_crash(line):
    """Detect crash notification."""
    pattern = r"Total Crashes:\s*(\d+)"
    match = re.search(pattern, line)
    if match:
        return int(match.group(1))
    return None


def parse_bitflip(line):
    """Detect bit flip event."""
    pattern = r"BIT FLIP DETECTED.*Jump:\s*(-?\d+)"
    match = re.search(pattern, line)
    if match:
        return int(match.group(1))
    return None


def calculate_availability(total_uptime_sec, crash_count, wdt_timeout_sec=2):
    """Calculate system availability percentage."""
    if total_uptime_sec == 0:
        return 100.0
    total_downtime = crash_count * wdt_timeout_sec
    total_time = total_uptime_sec + total_downtime
    return (total_uptime_sec / total_time) * 100


def main():
    # Parse arguments
    if len(sys.argv) >= 2:
        port = sys.argv[1]
    else:
        port = find_arduino_port()
        if not port:
            print("Error: Could not auto-detect Arduino port.")
            print("Available ports:")
            for p in serial.tools.list_ports.comports():
                print(f"  {p.device}: {p.description}")
            print("\nUsage: python3 fira_logger.py <port> [output.csv]")
            sys.exit(1)

    output_file = sys.argv[2] if len(sys.argv) >= 3 else f"fira_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

    print(f"""
╔══════════════════════════════════════════════════════════════╗
║         FIRA - Serial Data Logger & Analyzer                 ║
╚══════════════════════════════════════════════════════════════╝

Port: {port}
Baud: {BAUD_RATE}
Output: {output_file}

Press Ctrl+C to stop logging and generate report.
═══════════════════════════════════════════════════════════════
""")

    # Statistics tracking
    stats = {
        'start_time': datetime.now(),
        'last_counter': 0,
        'total_crashes': 0,
        'total_faults': 0,
        'bitflips': [],
        'max_uptime': 0,
        'samples': 0
    }

    try:
        # Open serial connection
        ser = serial.Serial(port, BAUD_RATE, timeout=1)
        time.sleep(2)  # Wait for Arduino reset

        # Open CSV file
        with open(output_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['timestamp', 'counter', 'uptime_sec', 'faults', 'crashes', 'bitflip_jump', 'raw_line'])

            print("Logging started. Waiting for data...\n")

            while True:
                try:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if not line:
                        continue

                    timestamp = datetime.now().isoformat()

                    # Print to console
                    print(line)

                    # Parse different message types
                    heartbeat = parse_heartbeat(line)
                    crash_count = parse_crash(line)
                    bitflip = parse_bitflip(line)

                    # Update statistics
                    counter = heartbeat['counter'] if heartbeat else ''
                    uptime = heartbeat['uptime'] if heartbeat else ''
                    faults = heartbeat['faults'] if heartbeat else ''
                    crashes = crash_count if crash_count else ''
                    bitflip_val = bitflip if bitflip else ''

                    if heartbeat:
                        stats['last_counter'] = heartbeat['counter']
                        stats['max_uptime'] = max(stats['max_uptime'], heartbeat['uptime'])
                        stats['total_faults'] = heartbeat['faults']
                        stats['samples'] += 1

                    if crash_count:
                        stats['total_crashes'] = crash_count

                    if bitflip:
                        stats['bitflips'].append(bitflip)

                    # Write to CSV
                    writer.writerow([timestamp, counter, uptime, faults, crashes, bitflip_val, line])
                    csvfile.flush()

                except KeyboardInterrupt:
                    break

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)

    finally:
        try:
            ser.close()
        except:
            pass

    # Generate report
    duration = (datetime.now() - stats['start_time']).total_seconds()
    availability = calculate_availability(stats['max_uptime'], stats['total_crashes'])

    print(f"""

═══════════════════════════════════════════════════════════════
                    RESEARCH DATA SUMMARY
═══════════════════════════════════════════════════════════════

Session Duration:     {duration:.1f} seconds
Data Samples:         {stats['samples']}
Maximum Uptime:       {stats['max_uptime']} seconds
Total Crashes:        {stats['total_crashes']}
Total Fault Injects:  {stats['total_faults']}
Bit Flips Detected:   {len(stats['bitflips'])}

───────────────────────────────────────────────────────────────
                    AVAILABILITY METRICS
───────────────────────────────────────────────────────────────

System Availability:  {availability:.2f}%
Mean Time To Recovery (MTTR): 2.0 seconds (WDT timeout)
Mean Time Between Failures: {stats['max_uptime'] / max(stats['total_crashes'], 1):.1f} seconds

───────────────────────────────────────────────────────────────
                    BIT FLIP ANALYSIS
───────────────────────────────────────────────────────────────
""")

    if stats['bitflips']:
        print(f"Bit flip magnitudes: {stats['bitflips']}")
        avg_jump = sum(abs(b) for b in stats['bitflips']) / len(stats['bitflips'])
        print(f"Average jump magnitude: {avg_jump:.1f}")
    else:
        print("No bit flips recorded (Attack Mode A not active?)")

    print(f"""
───────────────────────────────────────────────────────────────
                    HYPOTHESIS VALIDATION
───────────────────────────────────────────────────────────────

Hypothesis: "With WDT enabled, the system recovers within 2000ms,
             maintaining 90%+ availability."

Result: {"✓ VALIDATED" if availability >= 90 else "✗ NOT VALIDATED"}
        System achieved {availability:.1f}% availability

Data exported to: {output_file}
═══════════════════════════════════════════════════════════════
""")


if __name__ == '__main__':
    main()

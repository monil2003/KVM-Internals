#!/usr/bin/env bash

echo "======================================"
echo " DIY-Hypervisor Environment Check"
echo "======================================"

echo
echo "[1] CPU Virtualization Support"

if egrep -q "(vmx|svm)" /proc/cpuinfo; then
    echo "PASS: CPU supports hardware virtualization."
else
    echo "FAIL: Virtualization extensions not found."
    exit 1
fi

echo
echo "[2] KVM Device"

if [ -e /dev/kvm ]; then
    echo "PASS: /dev/kvm exists."
else
    echo "FAIL: /dev/kvm not found."
    exit 1
fi

echo
echo "[3] KVM Permissions"

if [ -r /dev/kvm ] && [ -w /dev/kvm ]; then
    echo "PASS: Current user can access /dev/kvm."
else
    echo "WARNING: Check kvm group permissions."
fi

echo
echo "[4] Kernel Modules"

lsmod | grep kvm

echo
echo "Environment looks good."

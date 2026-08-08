# KVM Hypercalls

This component implements a minimal userspace hypervisor directly using the Linux Kernel-based Virtual Machine (KVM) API.

The goal is to understand the basic mechanisms involved in hardware-assisted virtualization and guest-hypervisor communication, including VM creation, vCPU execution, guest memory mapping, VM exits, custom hypercalls, and guest address translation.

## Overview

The hypervisor creates a KVM virtual machine, allocates guest memory, loads a guest program, configures the virtual CPU, and executes the guest using the `KVM_RUN` interface.

The guest communicates with the hypervisor through x86 I/O instructions. These instructions cause the guest to exit to KVM, after which the userspace hypervisor handles the corresponding `KVM_EXIT_IO` event.

The implementation supports execution of the guest in multiple x86 modes:

- Real Mode
- Protected Mode
- 32-bit Paging
- 64-bit Long Mode

## Hypercalls

Several custom hypercalls are implemented using dedicated I/O ports.

### `HC_print8bit()`

Sends an 8-bit value through port `0xE9`.

The hypervisor receives the value through a `KVM_EXIT_IO` and writes it to the terminal.

This provides the basic guest-to-hypervisor communication mechanism.

### `HC_print32bit()`

Sends a 32-bit integer through port `0xEA`.

The hypervisor reads the value from the KVM I/O exit structure and prints it followed by a newline.

### `HC_numExits()`

Uses port `0xEB` to request the number of VM exits that have occurred during guest execution.

The hypervisor maintains the exit count and supplies the value back to the guest through an I/O-in operation.

### `HC_printStr()`

Uses port `0xEC` to pass the guest virtual address of a string to the hypervisor.

The hypervisor uses the supplied guest address to access the string in guest memory and prints the complete string.

Unlike printing a string character-by-character through `HC_print8bit()`, this operation requires only one guest-to-hypervisor I/O exit.

### `HC_numExitsByType()`

Uses port `0xED` to obtain a string containing the number of I/O-in and I/O-out exits.

The hypervisor constructs a string of the form:

```text
IO in: x
IO out: y
```

The guest receives the address of this string and can subsequently pass it to `HC_printStr()`.

### `HC_gvaToHva()`

Uses ports `0xEE` and `0xEF` to request translation of a Guest Virtual Address (GVA) into a Host Virtual Address (HVA).

The hypervisor performs the translation using the KVM address translation interface.

For an invalid GVA, the hypervisor reports:

```text
Invalid GVA
```

and returns `0`.

Since the guest operates with 32-bit values while the host virtual address can be larger than 32 bits, the implementation uses the lower 32 bits of the HVA.

## I/O Port Layout

| Port | Purpose |
|------|---------|
| `0xE9` | `HC_print8bit()` |
| `0xEA` | `HC_print32bit()` |
| `0xEB` | `HC_numExits()` |
| `0xEC` | `HC_printStr()` |
| `0xED` | `HC_numExitsByType()` |
| `0xEE` | GVA supplied to `HC_gvaToHva()` |
| `0xEF` | HVA returned from `HC_gvaToHva()` |

## Execution Modes

The project demonstrates guest execution under different x86 configurations.

### Real Mode

```bash
./simple-kvm
```

The real-mode guest is provided by `guest16.s`.

### Protected Mode

```bash
./simple-kvm -s
```

### 32-bit Paging

```bash
./simple-kvm -p
```

### 64-bit Long Mode

```bash
./simple-kvm -l
```

The appropriate guest image is selected and configured by the hypervisor based on the execution mode requested.

## VM Execution

The main execution loop uses:

```c
ioctl(vcpu->vcpu_fd, KVM_RUN, 0);
```

KVM executes the virtual CPU until an event requiring userspace handling occurs.

The hypervisor then examines:

```c
vcpu->kvm_run->exit_reason
```

For hypercalls, the relevant exit reason is:

```text
KVM_EXIT_IO
```

The I/O exit structure provides information such as:

- Direction of the I/O operation
- I/O port
- Data size
- Offset of the I/O data within the `kvm_run` structure

This information is used to dispatch the corresponding hypercall.

## Guest Memory

The hypervisor allocates memory for the guest and registers it with KVM.

The guest programs execute using this memory as their address space.

The hypervisor also accesses guest memory directly when implementing operations such as:

- `HC_printStr()`
- GVA-to-HVA translation
- Reading and validating guest state

## Files

### `simple-kvm.c`

Main userspace hypervisor implementation.

It handles:

- Opening `/dev/kvm`
- Creating the VM
- Creating the vCPU
- Allocating guest memory
- Configuring CPU state
- Loading guest programs
- Running the guest
- Handling VM exits
- Implementing the hypercalls
- Configuring different x86 execution modes

### `guest.c`

Guest-side implementation of the hypercalls.

It contains the guest functions used to communicate with the hypervisor through x86 I/O instructions.

### `guest16.s`

Small 16-bit guest program used for real-mode execution.

### `guest.ld`

Linker script used to construct the guest image.

### `payload.ld`

Linker script used when combining the guest images into the payload used by the hypervisor.

### `Makefile`

Build rules for compiling the hypervisor and guest programs and constructing the final executable.

## Building

From this directory:

```bash
make
```

## Running

Run the default real-mode configuration:

```bash
./simple-kvm
```

Run protected mode:

```bash
./simple-kvm -s
```

Run 32-bit paging:

```bash
./simple-kvm -p
```

Run 64-bit long mode:

```bash
./simple-kvm -l
```

## Example Output

A protected-mode execution produces output similar to:

```text
Testing protected mode
Hello 695!
2048
4294967295
CS695 Assignment 2
14
16
...
IO in: 3
IO out: 16
...
```

The exact HVA values and exit counts can vary between executions and environments.

## Concepts Explored

This component focuses on understanding:

- Linux KVM userspace API
- `/dev/kvm`
- Virtual machine creation
- vCPU creation and execution
- Guest memory mapping
- `KVM_RUN`
- `KVM_EXIT_IO`
- VM exit handling
- Hardware-assisted virtualization
- Guest-hypervisor communication
- Custom hypercall design
- Real Mode
- Protected Mode
- 32-bit Paging
- 64-bit Long Mode
- Guest virtual memory
- `KVM_TRANSLATE`

## Learning Objective

The implementation intentionally stays close to the KVM interface to make the virtualization mechanisms visible.

Rather than treating KVM as a black-box virtual machine, this component uses it as the hardware-assisted execution layer while implementing the userspace hypervisor logic explicitly.
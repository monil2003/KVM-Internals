# DIY Hypervisor

A minimal educational hypervisor built directly on top of the Linux Kernel-based Virtual Machine (KVM) API.

This project was developed to understand the internals of hardware-assisted virtualization by progressively building a userspace hypervisor that creates virtual machines, configures virtual CPUs, executes guest programs, handles VM exits, implements guest-to-hypervisor hypercalls, and coordinates multiple virtual machines.

The project works directly with the KVM userspace interface rather than relying on a complete virtual machine monitor such as QEMU.

## Motivation

Virtualization abstractions often hide the mechanisms involved in running guest code. This project explores those mechanisms directly through Linux KVM.

The project focuses on understanding:

- KVM virtual machine creation
- Virtual CPU creation and execution
- Guest memory mapping
- x86 execution modes
- VM exits
- I/O-based guest-to-hypervisor communication
- Custom hypercalls
- Guest virtual address translation
- Multi-VM execution
- VM scheduling
- Producer-consumer coordination
- Circular buffers
- Hypervisor-mediated shared memory

The implementation is organized into two components that progressively build these concepts.

## Project Structure

```text
DIY-Hypervisor/
├── Makefile
├── README.md
├── kvm-hypercalls/
│   ├── Makefile
│   ├── README.md
│   ├── simple-kvm.c
│   ├── guest.c
│   ├── guest16.s
│   ├── guest.ld
│   └── payload.ld
├── vm-scheduling/
│   ├── Makefile
│   ├── README.md
│   ├── emu.c
│   ├── emu1.c
│   ├── emu2.c
│   ├── emu3.c
│   ├── emu4.c
│   ├── guest*.c
│   ├── guest*.s
│   ├── guest.ld
│   ├── payload.ld
│   ├── sched1.txt
│   └── sched2.txt
└── scripts/
    └── check-kvm.sh
```

## KVM Hypercalls

The `kvm-hypercalls` component implements a minimal KVM-based hypervisor and explores guest-to-hypervisor communication through custom hypercalls.

It covers:

- Real Mode
- Protected Mode
- 32-bit Paging
- 64-bit Long Mode
- KVM VM and vCPU management
- Guest memory mapping
- VM exit handling
- I/O exits
- Guest-to-hypervisor hypercalls
- VM exit counting
- VM exit statistics
- Guest Virtual Address to Host Virtual Address translation

See [`kvm-hypercalls/README.md`](kvm-hypercalls/README.md) for details.

## Multi-VM Scheduling

The `vm-scheduling` component builds on the KVM concepts from the first component and introduces multiple guest VMs.

It progressively explores:

- Producer-consumer VMs
- VM-to-VM communication through the hypervisor
- Bursty scheduling
- Batched production and consumption
- Randomized workloads
- Schedule-file-driven VM scheduling
- Circular producer-consumer buffers
- Hypervisor-mediated virtual shared memory

See [`vm-scheduling/README.md`](vm-scheduling/README.md) for details.

## Building

Build the complete project from the repository root:

```bash
make
```

Clean generated build artifacts:

```bash
make clean
```

Individual components can also be built independently:

```bash
cd kvm-hypercalls
make
```

```bash
cd vm-scheduling
make
```

## Requirements

The project requires a Linux system with KVM support.

The KVM device should be available at:

```text
/dev/kvm
```

Basic KVM environment checks can be performed using:

```bash
./scripts/check-kvm.sh
```

## Learning Outcomes

This project provides hands-on experience with:

- Linux KVM userspace APIs
- Hardware-assisted virtualization
- Virtual machine and vCPU lifecycle
- x86 execution modes
- Guest memory mapping
- VM exits and I/O exits
- Guest-hypervisor communication
- Custom hypercall design
- Guest address translation
- Multi-VM scheduling
- Producer-consumer synchronization
- Circular buffer management
- Hypervisor-mediated shared-memory emulation

## Acknowledgement

This project was developed as an independent learning exercise inspired by a graduate-level course project on Linux KVM and hardware-assisted virtualization.

The course project provided the initial motivation and progression of concepts involving KVM, hypercalls, multiple VMs, scheduling, and producer-consumer virtualization. The work was subsequently organized and developed as a standalone educational project focused on understanding hypervisor internals.

Course project reference:

https://docs.google.com/document/d/e/2PACX-1vQcjg6HkPm47_L1DrLH8X6EqhRqr_lF2KmH8fk3fCnOPspRIzKLeVTiPoUJ9CGn3-FIrr5xilD96tN-/pub
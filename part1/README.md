# DIY Hypervisor – Understanding Linux KVM Internals

DIY Hypervisor is an educational project built using the Linux Kernel-based Virtual Machine (KVM) API to explore the internals of hardware-assisted virtualization. The goal is to understand how a userspace hypervisor creates and manages virtual machines, handles VM exits, implements custom hypercalls, performs guest memory translation, and eventually schedules multiple guest VMs.

This repository is being developed incrementally as a learning project. The current implementation focuses on hypervisor fundamentals and custom hypercalls, with later stages extending the hypervisor with multi-VM scheduling and communication mechanisms.

---

## Current Progress

### Part 1 — Hypercalls and VM Internals

- Built a minimal userspace hypervisor using the Linux KVM API
- Booted guests in
  - Real Mode
  - Protected Mode
  - 32-bit Paging
  - 64-bit Long Mode
- Implemented hardware-assisted hypercalls
- Counted and classified VM exits
- Implemented Guest Virtual Address (GVA) to Host Virtual Address (HVA) translation using `KVM_TRANSLATE`

### Planned Extensions

- Multi-VM execution
- Custom VM scheduler
- Producer–consumer communication between guest VMs
- Bursty and non-deterministic scheduling policies

---

## Implemented Hypercalls

| Hypercall | Description |
|-----------|-------------|
| `HC_print8bit()` | Prints a single character from the guest using an I/O hypercall |
| `HC_print32bit()` | Transfers and prints a 32-bit integer from the guest |
| `HC_numExits()` | Returns the total number of VM exits encountered during execution |
| `HC_printStr()` | Prints an entire guest string using a single VM exit |
| `HC_numExitsByType()` | Returns the number of IO-IN and IO-OUT VM exits |
| `HC_gvaToHva()` | Translates a Guest Virtual Address into the corresponding Host Virtual Address using `KVM_TRANSLATE` |

---

## Concepts Explored

- Linux KVM userspace API
- Virtual machine creation
- Virtual CPU (vCPU) management
- Guest memory mapping
- VM exits
- Hardware-assisted hypercalls
- Guest ↔ Hypervisor communication
- I/O virtualization
- Guest virtual memory translation
- Protected Mode initialization
- Paging
- Long Mode initialization

---

## Repository Structure

```
DIY-Hypervisor/
├── part1/
│   ├── Makefile
│   ├── README.md
│   ├── guest.c
│   ├── guest16.s
│   ├── guest.ld
│   ├── payload.ld
│   └── simple-kvm.c
│
└── part2/      # Multi-VM scheduling (under development)
```

---

## Building

```bash
cd part1
make
```

---

## Running

```bash
./simple-kvm       # Real Mode
./simple-kvm -s    # Protected Mode
./simple-kvm -p    # 32-bit Paging
./simple-kvm -l    # 64-bit Long Mode
```

---

## Learning Outcomes

Through this project I gained practical experience with:

- Building a userspace hypervisor using the Linux KVM API
- Managing guest execution through VM exits
- Designing hardware-assisted hypercalls
- Accessing guest memory from the hypervisor
- Translating Guest Virtual Addresses using `KVM_TRANSLATE`
- Understanding CPU execution modes and memory virtualization

---

## Acknowledgements

The initial implementation was inspired by a graduate-level virtualization assignment focused on Linux KVM and hardware-assisted virtualization. This repository extends those ideas into a personal learning project with additional documentation, code organization, and planned extensions for virtual machine scheduling and communication.

**Reference**

https://docs.google.com/document/d/e/2PACX-1vQcjg6HkPm47_L1DrLH8X6EqhRqr_lF2KmH8fk3fCnOPspRIzKLeVTiPoUJ9CGn3-FIrr5xilD96tN-/pub
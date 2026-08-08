# Multi-VM Scheduling

This component extends the KVM virtualization environment to multiple guest VMs and explores how a userspace hypervisor can coordinate their execution, exchange data between isolated guest memories, and emulate shared-buffer communication.

The implementation progresses from a basic producer-consumer system to bursty scheduling, batched communication, and finally randomized producer-consumer execution over a finite virtual circular buffer.

## Overview

The four main implementations progressively introduce additional scheduling and communication mechanisms:

```text
emu1.c
  |
  v
Basic Producer-Consumer
  |
  v
emu2.c
  |
  v
Bursty VM Scheduling
  |
  v
emu3.c
  |
  v
Batched Producer-Consumer
  |
  v
emu4.c
  |
  v
Randomized Scheduling + Virtual Shared Buffer
```

## 2.1 Basic Producer-Consumer

`emu1.c` runs two guest VMs:

- Producer VM
- Consumer VM

The producer generates one value per hypercall. The hypervisor receives the produced value and provides it to the consumer VM through the corresponding I/O exits.

The implementation demonstrates basic coordination between two independently executing KVM virtual machines.

Run:

```bash
./emu1
```

## 2.2 Bursty VM Scheduling

`emu2.c` changes the scheduling policy so that the producer executes several production operations before the consumer gets a chance to run.

The producer produces three values before the consumer is scheduled to consume them. The cycle then repeats.

This demonstrates that the userspace hypervisor can explicitly control the order in which multiple VMs receive CPU execution.

Run:

```bash
./emu2
```

## 2.3 Batched Producer-Consumer

`emu3.c` changes the communication granularity from individual values to batches.

The producer generates five values in its local buffer without trapping to the hypervisor for each individual value. After producing the batch, it traps once and passes the guest address of the buffer to the hypervisor.

The hypervisor:

1. Reads the producer's buffer.
2. Prints the produced values.
3. Copies the buffer contents into the consumer VM's memory.
4. Schedules the consumer VM.

The consumer then processes the batch and reports its buffer back to the hypervisor.

Run:

```bash
./emu3
```

Example:

```text
VMFD: 4 Produced Values: 0 1 2 3 4
VMFD: 6 Consumed Values: 0 1 2 3 4
```

## 2.4 Randomized Scheduling and Virtual Shared Buffer

`emu4.c` introduces a finite virtual shared buffer and non-deterministic producer and consumer workloads.

The logical buffer contains 20 elements.

The producer and consumer do not directly share memory. Each VM maintains a local copy of the buffer, while the hypervisor maintains the authoritative logical state and copies data between VM memories when required.

Conceptually:

```text
             Producer VM
             local buffer
                  |
                  | state + data
                  v
             Hypervisor
          virtual shared buffer
                  |
                  | state + data
                  v
             Consumer VM
             local buffer
```

The hypervisor maintains the logical state of the producer-consumer buffer and coordinates access between the two VMs.

## Buffer State

The implementation uses two pointers:

- `prod_p` — location where the last produced element was stored
- `cons_p` — location of the next element to be consumed

Initially:

```text
prod_p = -1
cons_p = -1
```

This represents an empty buffer.

The buffer is treated as a circular buffer with a capacity of 20 elements.

The producer advances `prod_p` while ensuring that it never overwrites an element that has not yet been consumed.

The consumer advances `cons_p` while ensuring that it never consumes an element that is unavailable.

This prevents:

- Buffer overflow
- Buffer underflow

## Randomized Workload

The guest VMs use the x86 `rdtsc` instruction as the source for generating pseudo-random values.

A scheduling iteration chooses a value in the range:

```text
[0, 10]
```

which determines how many elements the VM attempts to produce or consume during that iteration.

The actual number processed can be limited by the available space or available elements in the buffer.

## Scheduling

The scheduling order for `emu4` is provided through a text file.

For example:

```bash
./emu4 sched1.txt
```

or:

```bash
./emu4 sched2.txt
```

The schedule contains a sequence of VM identifiers:

```text
1 -> Producer VM
2 -> Consumer VM
```

Each entry represents one scheduling opportunity.

The hypervisor reads the next scheduling decision and executes the corresponding VM.

## Scheduling Flow

For each scheduling opportunity:

```text
1. Hypervisor selects a VM from the schedule
2. VM reads the current producer-consumer state
3. VM performs its production or consumption
4. VM reports the updated state
5. Hypervisor updates its logical buffer state
6. Hypervisor copies buffer contents to the other VM when required
7. The next scheduled VM receives its updated state
```

This separates VM scheduling from the internal workload of each guest.

## Virtual Shared Buffer

The buffer is not physically shared between the guest VMs.

Instead, the hypervisor emulates a shared buffer by:

- Maintaining the logical buffer state
- Tracking producer and consumer pointers
- Reading guest buffer contents
- Copying data between guest memory regions
- Supplying updated buffer state to the scheduled VM

This allows isolated KVM guests to behave as though they are communicating through a shared producer-consumer buffer.

## Example Output

A typical execution has the following structure:

```text
HYPVSR: []
VMFD: 5 Produced 5 Values: 8 6 4 2 3
HYPVSR: [8 6 4 2 3]

VMFD: 7 Consumed 3 Values: 8 6 4
HYPVSR: [2 3]

VMFD: 5 Produced 2 Values: 6 5
HYPVSR: [2 3 6 5]

VMFD: 7 Consumed 1 Values: 2
HYPVSR: [3 6 5]
```

`HYPVSR` represents the hypervisor's current logical view of the virtual shared buffer.

The exact values depend on the guest-generated pseudo-random sequence and the selected scheduling file.

## Files

### `emu.c`

Initial multi-VM KVM environment used as the starting point for the multi-VM experiments.

### `emu1.c`

Implements basic producer-consumer coordination between two VMs.

### `emu2.c`

Introduces bursty scheduling, allowing the producer to run for multiple production operations before the consumer is scheduled.

### `emu3.c`

Introduces batched producer-consumer communication using five-element arrays.

### `emu4.c`

Implements randomized producer-consumer workloads, schedule-file-driven VM scheduling, and hypervisor-mediated virtual shared-buffer coordination.

### `guest1a.s` / `guest1b.s`

Guest programs used by the basic producer-consumer implementation.

### `guest2a.s` / `guest2b.s`

Guest programs used for bursty scheduling.

### `guest3a.c` / `guest3b.c`

Guest programs used for five-element batched production and consumption.

### `guest4a.c` / `guest4b.c`

Guest programs used for randomized production and consumption over the virtual circular buffer.

### `guesta.s` / `guestb.s`

Initial guest programs used by the multi-VM environment.

### `guest.ld`

Linker script used to construct guest images.

### `payload.ld`

Linker script used to construct the payload used by the hypervisor.

### `sched1.txt`

Scheduling sequence used by `emu4`.

### `sched2.txt`

Alternative scheduling sequence used by `emu4`.

### `Makefile`

Build rules for the multi-VM experiments and guest programs.

## Building

From this directory:

```bash
make
```

## Running

Initial multi-VM environment:

```bash
./emu
```

Basic producer-consumer:

```bash
./emu1
```

Bursty scheduling:

```bash
./emu2
```

Batched producer-consumer:

```bash
./emu3
```

Randomized virtual shared buffer:

```bash
./emu4 sched1.txt
```

or:

```bash
./emu4 sched2.txt
```

## Concepts Explored

This component focuses on:

- Multiple KVM virtual machines
- Multi-VM execution
- Userspace VM scheduling
- KVM I/O exits
- Guest-hypervisor communication
- Producer-consumer coordination
- Batched hypercalls
- Circular buffers
- Buffer overflow prevention
- Buffer underflow prevention
- Guest memory copying
- Hypervisor-mediated shared-memory emulation
- Deterministic VM scheduling
- Non-deterministic guest workloads
- Randomized guest execution
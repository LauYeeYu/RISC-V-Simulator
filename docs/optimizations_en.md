# Optimizations

Since many materials about the optimization of CPU is f**king vague about the
actual meaning of the word and there is full of abbreviations, I decided to
write a little tutorial about the optimization of CPU.

The simple version of CPU is quite capable of handling the workload every
algorithm, but the CPU cannot perform 100% of its ability.  An important
reason is that different CPU section has different usage, and during
execution, most sections are idle.  Thus, the major optimization lies in
how to fully utilize these sections.

The first idea is to make these sections do things simultaneously.  To
achieve that goal, we need to split the operation of each instruction into
multiple parts.  That is called ***Pipeline***.

The second idea is to do things out of order.  Many instructions do not
depend on other instructions.  Thus, we can do them in any order.  Another
merit about it is that, we can boost the efficiency even by simply adding
the number of ALU. That is called ***Tomasulo architecture***.

On top these optimizations, we can also boost the efficiency of CPU by
using the ***Cache*** and predicting the branch that will be taken.

## 5-Stage Pipeline

### The five stages of the pipeline
- Instruction Fetch (IF)
  - Fetch the instruction on the address where the PC register is pointed
    from the memory.
  - PC register is incremented by 4.
- Instruction Decode (ID)
  - Decode the instruction.
  - The instruction is divided into different parts.
  - The data is get from the register.
- Execute (EXE)
  - ALU executes the instruction.
- Data Memory Access (DM)
- Push the data into memory or get the data from memory.
- Write Back (WB)
  - Write the result to the register.

### HAZARDS

***Structural Hazards*** (caused by hardware)
1. Loading the data from memory while writing will cause the hardware hazard
   of memory.

   Solution: Stall for this clock cycle.

2. Reading the data from register while writing will cause the hardware hazard
   of register.

   Solution: Stall for this clock cycle.

***Data Hazards*** (cause by data dependency)
1. The data to be used might be changed by the other instruction before
   The execution.

   Solution: Stall for this clock cycle. Use *forwarding* strategy to
   minimize the time of stall.

### Forwarding
Forwarding is a strategy to minimize the time of stall.  It is used when
the data to be used might be changed by the other instruction before the
execution.

## Tomasulo Architecture
The Tomasulo architecture is a pipeline architecture that allows executing
instructions out of order.  This architecture uses a *register file*,
*Reservation Stations (RS)* and *Reorder Buffer (RoB)* to make sure that
the behaviour is the same as defined.

### How it Works
Obviously, simple executing the instructions out of order will virtually
cause the wrong behaviour.  The problem is that if we execute these
instructions out of order, the state of storage media might not be what we
expect.  In this context, the storage media refers to two things — the
registers and the memory.

To solve this problem, the Tomasulo architecture came up with a quite clever
idea. What if we commit these modifications in the time order while still
keeping executing instructions out of order?  Thus, the *reorder buffer*
is invented.  Whenever we need to change the state of the registers, we have
to commit it in order.  The *reorder buffer* is a buffer that stores the
state of instructions that are not yet committed.  If the top element is
ready to commit, the new value is committed to the register, and then that
element is removed from the buffer.

For the memory part, the *Load Store Buffer* is invented.  The *Load Store
Buffer* is a buffer that stores the information of loading or storing
instructions.

But there is another problem.  When cannot tell whether the value from the
register depends on the previous instruction or not.  Thus, we have to
record this data in the *register file*.  The *register file* records the
latest dependency of each register, indicating which instruction the new
instruction depends on.  To keep the state and data of each instruction,
the *reservation station* is invented.  The *reservation station* stores
the information of the pending or executing instructions.

As for the data transmission, the *common data bus (CDB)* is used.  Whenever
a result is calculated, we need to send the result to every part through the
CDB.

### Components of the Tomasulo Architecture
- Register File
  - The register file is a table that records the latest dependency of each
    register.
- Reservation Stations (RS)
  - The reservation stations are a table that records the pending or executing
    instructions.
- Reorder Buffer (RoB)
  - The reorder buffer is a buffer that records the state of instructions
    that are not yet committed.
  - MUST follow the order of the instructions.
- Load Store Buffer (LSB)
  - The load store buffer is a buffer that records the information of
    loading or storing instructions.
  - MUST follow the order of the instructions.
- Instruction Unit
  - The instruction unit decodes and issues the instructions. 
- Common Data Bus (CDB)
  - The common data bus is a bus that sends the data to the other parts.

### Behaviour of each Kind of Instruction
First, every instruction is decoded by the instruction unit.

- Load Instruction
  - Push the data into the load store buffer.
  - Check the dependency of the register.  If the register is not ready,
    stall the instruction and keep checking whether the data can be
    decided. If the data is ready, get the data from the memory and
    put the data into the reorder buffer.
- Store Instruction
  - Push the data into the load store buffer.
  - Check the dependency of the register.  If the register is not ready,
    stall the instruction and keep checking whether the data can be
    decided. If the data is ready, write the data into the memory.
- Arithmetic or Logical Instruction
  - Check the dependency of the register.  If the register is not ready,
    stall the instruction and keep checking whether the data can be
    decided. If the data is ready, push the data into the ALU.
  - After the data is processed by the ALU, transmit the result through
    the common data bus.
  - Eventually, the result is written to the register when this instruction
    becomes the top element in the reorder buffer.
- Branch Instruction
  - The prediction unit (predictor) predict the answer and instruction unit
    keeps decoding at the predicted destination.
  - Store the address of the place where the CPU is not predicted and the
    predicted answer in the reorder buffer.
  - Check the dependency of the register.  If the register is not ready,
    stall the instruction and keep checking whether the data can be
    decided. If the data is ready, push the data into the ALU.
  - After the data is processed by the ALU, transmit the result through
    the common data bus.
  - When committing, the reorder buffer checks whether the prediction is
    correct.  If correct, just do nothing but move on to the next
    instruction. If not correct, clear the pipeline and set the PC to the
    recorded address.
- Jump Instruction
  - Check the dependency of the register.  If the register is not ready,
    stall the instruction and keep checking whether the data can be
    decided. If the data is ready, push the data into the address ALU and
    set the PC to the result.

### My Implementation
Since there is always congestion in the CDB, I think we should link these
part with a dedicated wire.

The following picture is the architecture of my implementation.

```text
Instruction
   Queue                          Registers
 +------+      +----+----+----+----+----+----+-----+----+----+----+----+----+
 |------|<---->| PC |    |    |    |    |    | ... |    |    |    |    |    |
 |------|      +----+----+----+----+----+----+-----+----+----+----+----+----+
 |------|<------+                                                          ^
 |------|<------|--------------------------------------------------------+ |
 |------|<------|-----+                   Register File                  | |
 |------|   +------+  |  +----+----+----+----+-----+----+----+----+----+ | |
 +------+   |------|  |  |    |    |    |    | ... |    |    |    |    | | |  Reorder
    ^      L|------|  |  +----+----+----+----+-----+----+----+----+----+ | |  Buffer
    |      S|------|  |                      ^                           v v  (RoB)
    |      B|------|  |                      |                          +-------------+
    | ALU<->|------|<-------------------------------------------------->|-------------|
    |       +------+  v Reservation Stations v                          |-------------|
    |          |   +--------------------------+                         |-------------|
    |          |   |--------------------------|<----------------------->|-------------|
    |          |   |--------------------------|  +--------------------+ |-------------|
    |          |   |--------------------------|  |                    | |-------------|
    |          |   |--------------------------|  |                    | |-------------|
    |          |   |--------------------------|--|        ALU         | |-------------|
    |          |   |--------------------------|  |                    | |-------------|
    |          |   |--------------------------|  |                    | |-------------|
    |          |   +--------------------------+  +--------------------+ +-------------+
    v          v
+-------------------------------------------------------------------------------------+
|                                      Memory                                         |
+-------------------------------------------------------------------------------------+
```

## Branch Prediction
Branch prediction will make a great difference when there is a lot of
branches.  A typical case that can show the advantage of the branch
prediction is loop.  During the loop, the program will often go through
the same loop again and again.  With a predictor, we can predict the branch
that will be hit and then continue processing instructions.  Therefore,
time is saved.  In the condition of great loops, the branch prediction
will reduce the number of branches by a half or more.

Typically, a predictor will predict the answer of the branch according to
the history data.  The predicted result will be noted.  Then the instruction
unit will decode the instruction at the predicted address and do things as
if there is no branches.  When the real branch is calculated, we need to
check whether the prediction is correct or not.  If correct, we just move
to the next instruction.  If not, we need to clear the pipeline and set
the PC to another address.

## Cache
The cache is a storage that hold only part of the data.  Cache is used to
resolve the problem that the memory access is too slow.  Therefore, we use
cache to speed up the memory access.

Cache serves as a buffer between the memory and the CPU.  For the frequently
visited data, we push it into the cache.  When we need the data, instead of
getting the data from memory, we can get it from the cache.  When we need to
write the data to memory, we can just write the data to the cache.  This may
cause the data inconsistency.  Therefore, we use an extra bit called *dirty
point* to indicate whether the data is modified or not.  If the *dirty* data
must be removed from the cache, we need to write the data back to the memory.

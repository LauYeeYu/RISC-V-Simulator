# Optimizations

Since many materials about the optimization of CPU is f**king vague about the
actual meaning of the word and there is full of abbreviations, I decided to
write a little tutorial about the optimization of CPU.

## Pipeline

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

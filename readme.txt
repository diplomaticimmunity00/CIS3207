Overview:
    This program simulates the way processes are handled by a computer system
    comprised of a combination of a CPUs and storage disks. The arrivals and departures 
    of these processes  to and from each component are abstracted into structs
    called events. The random arrival of events
    drive the simulation as they would in a real computer system.

Code detail/data structures:
    Time:
        This program uses an arbitrary time unit to represent relative time spent by a process on
        each component of the system. The program's global clock is advanced whenever a process begins
 execution on a CPU. In this simulation, execution does not wait for IO to finish. To simulate this, 
 a process' I/O 
        on a disk will not advance time.
    Process:
        A process is the most basic component of the simulation. A process is represented by
        a numeric ID, a cpu time, and an IO time. These times represent relative time spent on
        each respective component type.

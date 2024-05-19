#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// -------------- Types declarations --------------

typedef struct Process
{
    struct Process* next;
    struct Process* prev;
    int procSize;         // Process size in KB
    int startAU;          // Start Allocation Unit
    int endAU;            // End Allocation Unit
    int totalAUSize;      // Process size in Allocation Unit
    char procName;        // Process name
} Process;

typedef struct Memory
{
    struct Process* processes;
    int totalSpace;            // Total space that memory have
    int au;                    // The size of allocation unit
} Memory;

Memory* Memory_new(int totalSpace, int au);
int Memory_sizeToAU(Memory* m, int size);
int Memory_addProcess(Memory* m, int procSize, char procName);
int Memory_removeProcess(Memory* m, char procName);
void Memory_print(Memory* m);
void Memory_free(Memory* m);

Process* Process_new(int procSize, int startAU, int endAU, int totalAUSize, char procName);
int Process_isEmpty(Process* p);
void Process_free(Process* p);
void Process_freeAll(Process* p);

// -------------- Memory --------------

Memory* Memory_new(int totalSpace, int au)
{
    Memory* m = (Memory*) malloc(sizeof(Memory));
    m->totalSpace = totalSpace;
    m->au = au;

    int totalAUSize = Memory_sizeToAU(m, totalSpace);
    m->processes = Process_new(totalSpace, 0, totalAUSize, totalAUSize, '-');

    return m;
}

int Memory_sizeToAU(Memory* m, int size)
{
    return ceil((double) size / m->au);
}

int Memory_addProcess(Memory* m, int procSize, char procName)
{
    Process** currProcess = &m->processes;
    int totalAUSize = Memory_sizeToAU(m, procSize);

    if (*currProcess == NULL)
    {
        *currProcess = Process_new(procSize, 0, totalAUSize, totalAUSize, procName);
        return 0;
    }

    while ((*currProcess) != NULL && !Process_isEmpty(*currProcess))
    {
        currProcess = &(*currProcess)->next;
    }

    if (*currProcess == NULL || (*currProcess)->totalAUSize < totalAUSize)
    {
        return 1;
    }

    int endAU = (*currProcess)->startAU + totalAUSize;

    if ((*currProcess)->procSize > procSize)
    {
        int remainingSize = (*currProcess)->procSize - procSize;
        int remainingTotalAUSize = Memory_sizeToAU(m, remainingSize);

        Process* remainingSpace = Process_new(remainingSize, endAU, (*currProcess)->endAU, remainingTotalAUSize, (*currProcess)->procName);

        remainingSpace->next = (*currProcess)->next;
        remainingSpace->prev = *currProcess;
        (*currProcess)->next = remainingSpace;
    }

    (*currProcess)->procSize = procSize;
    (*currProcess)->endAU = endAU;
    (*currProcess)->procName = procName;
    (*currProcess)->totalAUSize = totalAUSize;

    printf("[Memory_addProcess]: Added '%c' proccess\n", procName);

    return 0;
}

int Memory_removeProcess(Memory* m, char procName)
{
    Process* cp = m->processes;
    while (cp != NULL && cp->procName != procName)
    {
        cp = cp->next;
    }
    
    if (cp == NULL)
    {
        printf("[Memory_removeProcess]: '%c' proccess not found\n", procName);
        return 1;
    }

    cp->procName = '-';

    if (Process_isEmpty(cp->next))
    {
        Process* temp = cp->next;

        cp->procSize += cp->next->procSize;
        cp->totalAUSize = Memory_sizeToAU(m, cp->procSize);
        cp->endAU = cp->next->endAU;

        cp->next = cp->next->next;
        if (cp->next != NULL) cp->next->prev = cp;

        Process_free(temp);
    }

    if (Process_isEmpty(cp->prev))
    {
        Process** temp = &cp->prev;

        cp->procSize += (*temp)->procSize;
        cp->totalAUSize = Memory_sizeToAU(m, cp->procSize);
        cp->startAU = (*temp)->startAU;

        cp->prev = (*temp)->prev;
        if (cp->prev != NULL) (*temp)->prev->next = cp;

        *temp = cp;

        Process_free(*temp);
    }

    printf("[Memory_removeProcess]: Removed '%c' proccess\n", procName);
    return 0;
}

void Memory_print(Memory* m)
{
    Process* currProcess = m->processes;

    while (currProcess != NULL)
    {
        printf("[%c | %d | %dau] -> ", currProcess->procName, currProcess->startAU, currProcess->totalAUSize);
        currProcess = currProcess->next;
    }

    printf("\n");
}

void Memory_free(Memory* m)
{
    Process_freeAll(m->processes);
    free(m);
}

// -------------- Process --------------

Process* Process_new(int procSize, int startAU, int endAU, int totalAUSize, char procName)
{
    Process* p = (Process*) malloc(sizeof(Process));
    p->procSize = procSize;
    p->startAU = startAU;
    p->endAU = endAU;
    p->totalAUSize = totalAUSize;
    p->procName = procName;
    p->next = NULL;
    p->prev = NULL;

    return p;
}

int Process_isEmpty(Process* p)
{
    return p != NULL && p->procName == '-';
}

void Process_free(Process* p)
{
    free(p);
}

void Process_freeAll(Process* p)
{
    if (p == NULL) return;

    Process_freeAll(p->next);
    free(p);
}

// -------------- Main --------------

int main(void)
{
    int totalSize = 1024;
    int au = 2;

    printf("+--------------------------+\n");
    printf("Memory: %d\n", totalSize);
    printf("Allocation Unit: %d\n", au);
    printf("Strategy: First Fit\n");
    printf("+--------------------------+\n");

    Memory* mem = Memory_new(totalSize, au);
    Memory_print(mem);

    Memory_addProcess(mem, 20, 'A');
    Memory_print(mem);

    Memory_addProcess(mem, 20, 'B');
    Memory_print(mem);

    Memory_addProcess(mem, 20, 'C');
    Memory_print(mem);

    Memory_removeProcess(mem, 'B');
    Memory_print(mem);

    Memory_addProcess(mem, 10, 'D');
    Memory_print(mem);

    Memory_addProcess(mem, 20, 'E');
    Memory_print(mem);

    Memory_removeProcess(mem, 'C');
    Memory_print(mem);

    Memory_free(mem);
}

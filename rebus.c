#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAXW 8           
#define MAXL 18          

static char word[MAXW][MAXL + 1];   
static int  nwords;                
static char op;                     
static int  idx[26];                
static char name[10];               
static int  nlet;                   
static int  lead[10];               
static int  digit[10], used[10];    
static int  first[10];              
static long long found, nodes;     
static int  find_all;               

static int linear(void) { 
    return op == '+' || op == '-';
 }


static int wsign(int i)
{
    if (i == nwords - 1) 
    return -1;
    if (op == '-' && i > 0)
     return -1;
    return 1;
}

static int res_idx(void) { 
    return op == '/' ? 0 : nwords - 1; 
}

static int parse(const char *s)
{
    memset(idx, -1, sizeof idx);
    memset(lead, 0, sizeof lead);
    nwords = nlet = 0;
    op = 0;
    int len = 0, eq = 0;
    word[0][0] = 0;
    for (const char *p = s; ; p++) {
        if (*p >= 'A' && *p <= 'Z') {
            if (len >= MAXL)
             return 0;
            word[nwords][len++] = *p;
            word[nwords][len] = 0;
            if (idx[*p - 'A'] < 0) {
                if (nlet == 10) 
                return 0;
                idx[*p - 'A'] = nlet;
                name[nlet++] = *p;
            }
        } else if (*p == 0) {
            if (len == 0) 
            return 0;
            if (len > 1) 
            lead[idx[word[nwords][0] - 'A']] = 1;
            nwords++;
            break;
        } else if (*p && strchr("+-*/=", *p)) {
            if (len == 0) 
            return 0;
            if (len > 1) 
            lead[idx[word[nwords][0] - 'A']] = 1;
            nwords++;
            len = 0;
            if (eq) 
            return 0;                       
            if (*p == '=')
             eq = 1;
            else if (op == 0) 
            op = *p;
            else if (op != *p) 
            return 0;            
            if (nwords >= MAXW)
             return 0;
            word[nwords][0] = 0;
        } else if (*p != ' ') {
            return 0;
        }
    }
    if (!eq || op == 0 || nwords < 3) 
    return 0;
    if (op == '/' && nwords != 3) 
    return 0;
    if (op == '*' || op == '/') {                   
        int tot = 0;
        for (int i = 0; i < nwords; i++)
            if (i != res_idx()) tot += (int)strlen(word[i]);
        if (tot > 36) 
        return 0;
    }
    return 1;
}
static long long value(int w)
{
    long long v = 0;
    for (const char *p = word[w]; *p; p++) v = v * 10 + digit[idx[*p - 'A']];
    return v;
}

static int check_full(void)
{
    for (int i = 0; i < nlet; i++)
        if (lead[i] && digit[i] == 0)
         return 0;     
    if (linear()) {
        long long s = 0;
        for (int i = 0; i < nwords; i++) s += wsign(i) * value(i);
        return s == 0;
    }
    if (op == '*') {
        __int128 p = 1;
        for (int i = 0; i < nwords - 1; i++) p *= value(i);
        return p == value(nwords - 1);
    }
   
    return value(1) != 0 && (__int128)value(1) * value(2) == value(0);
}

static int record(void)
{
    if (found++ == 0) 
    memcpy(first, digit, sizeof digit);
    return !find_all;
}

static int rec(int k)
{
    nodes++;
    if (k == nlet) 
    return check_full() ? record() : 0;
    for (int d = 0; d < 10; d++) {
        if (used[d]) 
        continue;
        digit[k] = d;
        used[d] = 1;
        int stop = rec(k + 1);
        used[d] = 0;
        if (stop) 
        return 1;
    }
    return 0;
}
int solve_rebus(const char *in, char *out)
{
    if (!parse(in)) 
    return 0;
    found = nodes = 0;
    memset(used, 0, sizeof used);
    rec(0);
    if (!found) 
    return 0;
    for (; *in; in++)
        *out++ = (*in >= 'A' && *in <= 'Z') ? (char)('0' + first[idx[*in - 'A']]) : *in;
    *out = 0;
    return 1;
}
static double now_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

static const char *tests[] = {
    "BE + BE = MOO", "TWO + TWO = FOUR", "SEND + MORE = MONEY",
    "FORTY + TEN + TEN = SIXTY", "ELEVEN + NINE + FIVE + FIVE = THIRTY",
    "SATURN + URANUS + NEPTUNE + PLUTO = PLANETS",
    "CAT - DOG = ABC", "AB * AB = CDE", "AB / C = DE",
};

int main(int argc, char **argv)
{
    char out[256];
    if (argc > 1 && strcmp(argv[1], "-a") == 0) {
         find_all = 1; argv++; argc--; 
        }
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            double t0 = now_ms();
            int ok = solve_rebus(argv[i], out);
            double t1 = now_ms();
            if (ok) 
            printf("%s  =>  %s", argv[i], out);
            else   
             printf("%s  =>  решения нет", argv[i]);
            if (find_all) 
            printf("  (решений: %lld)", found);
            printf("  [%.3f мс, узлов: %lld]\n", t1 - t0, nodes);
        }
        return 0;
    }
    for (unsigned i = 0; i < sizeof tests / sizeof *tests; i++) {
        double best = 1e18;
        for (int r = 0; r < 5; r++) {          
            double t0 = now_ms();
            solve_rebus(tests[i], out);
            double dt = now_ms() - t0;
            if (dt < best) 
            best = dt;
        }
        printf("%-46s %10lld узлов %10.3f мс\n", tests[i], nodes, best);
    }
    return 0;
}

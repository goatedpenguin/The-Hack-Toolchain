load ScopeTest.asm,
output-file ScopeTest.out,
compare-to ScopeTest.cmp,
output-list RAM[0]%D1.6.1 RAM[5]%D1.6.1 RAM[6]%D1.6.1;
repeat 2000 {
    ticktock;
}
output;

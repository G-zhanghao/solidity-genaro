contract d_storage_test {
    function Test() public pure{
        d_storage d = 0x123;
        uint i = d;
    }
}
// ----
// Warning: (96-102): Unused local variable.

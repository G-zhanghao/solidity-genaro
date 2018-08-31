contract d_storage_test {
    function Test() public pure{
        address a = 0x123;
        d_storage d = d_storage(a);
    }
}
// ----
// Warning: (94-105): Unused local variable.

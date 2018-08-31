contract d_storage_test {
    function Test() public pure{
        d_storage d = 0x123;
        address a = address(d);
    }
}
// ----
// Warning: (96-105): Unused local variable.

contract d_storage_test {
    function Test() public view{
        uint[16] memory a;
        uint64 res;
        d_storage d = 0x123;
        res = a.read_version(d, 0x2, 0x3);
    }
}
// ----

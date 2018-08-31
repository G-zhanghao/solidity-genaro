contract d_storage_test {
    function Test() public pure{
        address a = 0x123;
        d_storage d = 0x123;
        assert(d == a);
    }
}
// ----

contract d_storage_test {
    function Test() public pure{
        d_storage d1 = 0x123;
        d_storage d2 = 0x122;
        assert(d1 > d2);
    }
}
// ----

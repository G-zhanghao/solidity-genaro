contract d_storage_test {
    function Test() public view{
        d_storage d = 0x123;
        d.ssize(0x1);
    }
}
// ----

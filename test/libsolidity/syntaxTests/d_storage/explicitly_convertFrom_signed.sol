contract d_storage_test {
    function Test() public pure{
        d_storage d = d_storage(-123);
    }
}
// ----
// Warning: (67-78): Unused local variable.

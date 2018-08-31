contract d_storage_test {
    function Test() public pure{
        d_storage d = -123;
    }
}
// ----
// TypeError: (67-85): Type int_const -123 is not implicitly convertible to expected type d_storage.

contract d_storage_test {
    function Test() public pure{
        d_storage d = 0.123;
    }
}
// ----
//  TypeError: (67-86): Type rational_const 123 / 1000 is not implicitly convertible to expected type d_storage. Try converting to type ufixed8x3 or use an explicit conversion.

contract d_storage_test {
    mapping(d_storage => uint) file;
    function Test() public {
        d_storage d = 0x123;
        file[d] = 0x99;
        //assert(d == a);
    }
}
// ----

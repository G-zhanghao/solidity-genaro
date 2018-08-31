contract test {
	function fun() public returns(address) { 
        d_storage d = 0x12345;
        address a = d; 
        return a;
    }
}
// ----
// Warning: (17-137): Function state mutability can be restricted to pure

def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')

    # Double.
    self.continue_to('d0')
    assert self.get_float('$d0') == 1.5
    self.continue_to('d1')
    assert self.get_float('$d1') == 2.5
    self.continue_to('d2')
    assert self.get_float('$d2') == 4.0

    # Single.
    self.continue_to('s0')
    assert self.get_float('$s0') == 1.5
    self.continue_to('s1')
    assert self.get_float('$s1') == 2.5
    self.continue_to('s2')
    assert self.get_float('$s2') == 4.0

    # High registers..
    self.continue_to('d28')
    assert self.get_float('$d28') == 1.5
    self.continue_to('d29')
    assert self.get_float('$d29') == 2.5
    self.continue_to('d30')
    assert self.get_float('$d30') == 4.0
    self.continue_to('d31')
    assert self.get_float('$d31') == 4.0


def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')

    self.continue_to('x0')
    self.sendline('set $x0 = 3')
    self.continue_to('x1')
    assert self.get_int('$x0') == 3
    assert self.get_int('$x1') == 2
    self.sendline('set $x30 = 3')
    self.continue_to('x29')
    assert self.get_int('$x29') == 1
    assert self.get_int('$x30') == 3
    self.continue_to('x30')
    assert self.get_int('$x30') == 2

    self.continue_to('d0')
    self.sendline('set $d0 = 3.5')
    self.continue_to('d1')
    assert self.get_float('$d0') == 3.5
    assert self.get_float('$d1') == 2.5
    self.sendline('set $d31 = 3.5')
    self.continue_to('d30')
    assert self.get_float('$d30') == 1.5
    assert self.get_float('$d31') == 3.5
    self.continue_to('d31')
    assert self.get_float('$d31') == 2.5

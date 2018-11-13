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

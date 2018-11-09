def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')
    self.continue_to('x1')
    self.sendline('set $x30 = 3')
    self.continue_to('x29')
    assert self.get_int('$x29') == 1
    assert self.get_int('$x30') == 3
    self.continue_to('x30')
    assert self.get_int('$x30') == 2

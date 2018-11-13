def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')
    self.continue_to('r0')
    self.sendline('set $r0 = 3')
    self.continue_to('r1')
    assert self.get_int('$r0') == 3
    assert self.get_int('$r1') == 2

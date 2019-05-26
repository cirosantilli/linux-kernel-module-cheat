def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')
    self.continue_to('op1')
    assert self.get_int('$r0') == 1
    self.continue_to('result')
    assert self.get_int('$r1') == 3

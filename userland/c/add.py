def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')
    self.continue_to('op1')
    assert self.get_int('i') == 1
    self.continue_to('op2')
    assert self.get_int('j') == 2
    self.continue_to('result')
    assert self.get_int('k') == 3

# A wrapper for the checker engine


eng_dir = "checker_engine.so"
from ctypes import *
ckr_lib = CDLL(eng_dir)

class ckr_pos(Structure):
    _fields_ = [('down', c_uint64), ('up', c_uint64), ('king', c_int64),
        ('ply_count', c_int)]

# Set the argument types and return types
ckr_lib.ckr_new_eng.argtypes = []
ckr_lib.ckr_new_eng.restype = c_void_p

# This is just plain free()
ckr_lib.free = ckr_lib.ckr_free_eng
ckr_lib.free.argtypes = [c_void_p]
ckr_lib.free.restype = None

ckr_lib.ckr_starting_pos.argtypes = []
ckr_lib.ckr_starting_pos.restype = ckr_pos

ckr_lib.ckr_serialize_pos.argtypes = [POINTER(ckr_pos)]
ckr_lib.ckr_serialize_pos.restype = c_void_p

ckr_lib.ckr_deserialize_pos.argtypes = [c_char_p]
ckr_lib.ckr_deserialize_pos.restype = ckr_pos

ckr_lib.ckr_make_move.argtypes = [c_void_p, POINTER(ckr_pos), c_char_p]
ckr_lib.ckr_make_move.restype = ckr_pos

class ckr_eng(object):
    def __init__(self):
        self.eng = ckr_lib.ckr_new_eng()
    def __del__(self):
        ckr_lib.free(self.eng)
    def make_move(self, pos, mov_str):
        # First convert it to compact byte sequence
        mov = c_buffer(32)
        ind = 0
        for i in mov_str.split(' '):
            index = int(i[0]) * 8 + int(i[2])
            mov[ind] = index
            ind += 1
        # for i in range(3):
        #     print(mov[i], end=' ')
        # print('')
        return ckr_lib.ckr_make_move(self.eng, byref(pos), mov)

def ckr_pos_to_str(self):
    ptr = ckr_lib.ckr_serialize_pos(byref(self))
    str = c_char_p(ptr).value
    ckr_lib.free(ptr)
    return str.decode('ascii')
ckr_pos.__str__ = ckr_pos_to_str

def ckr_pos_from_str(str, ply):
    res = ckr_lib.ckr_deserialize_pos(c_char_p(str.encode('ascii')))
    res.ply_count = ply
    return res

def print_ckr_pos(pos):
    pos_str = str(pos)
    for i in range(64):
        print(pos_str[i] * 2, end='')
        if i % 8 == 7:
            print('')


# eng = ckr_eng()
# start_pos = ckr_pos_from_str(
#     ' . . . .B . . .  . w . w. . . .  . . . .. . . .  . . . .. . . . ', 55)
# start_pos.ply_count = 0
# print(start_pos)
# print_ckr_pos(start_pos)
# pos_1 = eng.make_move(start_pos, '5,2 4,1')
# pos_2 = eng.make_move(pos_1, '2,5 3,6')
# print_ckr_pos(pos_1)
# print_ckr_pos(pos_2)

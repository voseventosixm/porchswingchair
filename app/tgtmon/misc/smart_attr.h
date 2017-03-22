// Mandatory Attributes
MAP_ITEM(lba_read,    0, 242)
MAP_ITEM(lba_written, 1, 241)
MAP_ITEM(ave_erase,   2, 167)
MAP_ITEM(max_erase,   3, 165)
MAP_ITEM(temperature, 4, 194)

// Optional attributes    
MAP_ITEM(life_left,   5, 248)
MAP_ITEM(spare_block, 6, 249)
MAP_ITEM(ecc_error,   7, 187)
MAP_ITEM(prog_fail,   8, 181)
MAP_ITEM(erase_fail,  9, 182)
MAP_ITEM(endurance,  10, 168)

// Debug attributes
MAP_ITEM(last_error, 11,  30)
MAP_ITEM(prog_sector,12,  31)
MAP_ITEM(life_typea, 13,  32)
MAP_ITEM(life_typeb, 14,  33)
MAP_ITEM(extcsd_ver, 15,  34)
MAP_ITEM(ave_mlc,    16,  35)
MAP_ITEM(max_mlc,    17,  36)
MAP_ITEM(ave_slc,    18,  37)
MAP_ITEM(max_slc,    19,  38)

MAP_ITEM(power_hour,  20,  9)
MAP_ITEM(power_count, 21,  12)

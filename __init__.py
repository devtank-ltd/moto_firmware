""" Moto binding package"""
## @package moto_firmware
#
# LoTI firmware wrapper
#

import sys

VERSION = 1.0

if sys.version_info.major == 3:
    ##
    # @brief Publically exportable modules
    #
    __all__ = [
        "binding",
    ]

    ##
    # @brief Import all variables in these modules into other namespaces
    #
    from .binding import io_board_py_t
    from .binding import debug_print as io_debug_print, set_debug_print as io_set_debug_print, get_debug_print as io_get_debug_print
else:
    from binding import io_board_py_t
    from binding import debug_print as io_debug_print, set_debug_print as io_set_debug_print, get_debug_print as io_get_debug_print

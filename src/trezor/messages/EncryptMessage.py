# Automatically generated by pb2py
import protobuf as p

class EncryptMessage(p.MessageType):
    FIELDS = {
        1: ('pubkey', p.BytesType, 0),
        2: ('message', p.BytesType, 0),
        3: ('display_only', p.BoolType, 0),
        4: ('address_n', p.UVarintType, p.FLAG_REPEATED),
        5: ('coin_name', p.UnicodeType, 0), # default=u'Bitcoin'
    }
    MESSAGE_WIRE_TYPE = 49

# decoding_utils.py
# Python 2 compatible (no dataclasses, no f-strings)

import re

DecErr = {1: "Invalid data header",
          2: "Invalid data trailer",
          20: "Invalid data for V792",
          21: "OV data for V792",
          22: "UN data for V792",
          23: "UN OV data for V792",
          30: "Invalid data for V792N",
          31: "OV data for V792N",
          32: "UN data for V792N",
          33: "UN OV data for V792N",
          40: "Invalid data for V775",
          41: "OV data for V775",
          42: "UN data for V775",
          43: "UB OV data for V775",
          50: "Invalid data for V775N",
          51: "OV data for V775N",
          52: "UN data for V775N",
          53: "UN OV data for V775N",
          99: "Invalid data type flag",
          111: "QDC Channel already seen",
          112: "TDC Channel already seen",
          74: "Expect evt trailer but no more data",
          75: "Failed checksum event size",
          254: "Unexpected 0xFE data word",
          # event header and trailer
          999: "Failed header sanity check",
          801: "....",
          810: "Invalid event trailer"
          }

def DiscardEvent(validitylist):
    fatals = [1, 2, 99, 111, 112, 74, 75, 254, 999, 810]
    discard = bool(set(fatals) & set(validitylist))
    return discard
    
def _mask(n):
    return (1 << n) - 1 if n else 0

def _get_bits(v, start, size):
    """Extract 'size' bits starting at bit 'start' (0 = LSB)."""
    return (v >> start) & _mask(size)

def _u32(v):
    return v & 0xFFFFFFFF

# ---- Event header decoded all together
def parse_evt_header(eh, verb = False): # eh is a list of 14 words(numbers) with different size
    """index word
    0  eventMarker
    1  eventNumber
    2  spillNumber
    3  headerSize
    4  trailerSize
    5  dataSize
    6  eventSize
    7  eventTimeSecs
    8  eventTimeMicrosecs
    9  triggerMask
    10 isPedMask
    11 isPedFromScaler
    12 sanityFlag
    13 headerEndMarker
    """
    
    if len(eh) != 14:
        return 999, {}
    if eh[0] != 0xccaaffee or eh[13] != 0xaccadead or eh[3] != 0xe or eh[4] != 0x1 or eh[6] != eh[3]+eh[4]+eh[5]: 
        return 999, {}

    evtnumber = eh[1]
    spillnumber = eh[2]
    
    datasize = eh[5]
    evttime = eh[7] + 1000000*eh[8]

    trigmask = eh[9]

    if verb:
        print("### Event header %d spill %d timeus %d trigger %d pay_size %d"%(evtnumber,spillnumber,evttime,trigmask,datasize))

    return 0, {"evtnumber": evtnumber, "evttime": evttime, "spillnumber": spillnumber, "trigmask": trigmask, "payloadsize": datasize}

# ----
def parse_evt_trail(et, verb = False): # et is a single 4-bytes word
    if et != 0xbbeeddaa:
        return 810, {}

    if verb:
        print('### Event trailer')
    return 0, {}
        
    

# ---- Bit layouts (LSB -> MSB) ----------------------------------------------

def parse_head(hd, verb = False):
    hd = _u32(hd)
    u = _get_bits(hd, 0, 8)
    nch = _get_bits(hd, 8, 6)
    zero = _get_bits(hd, 14, 2)
    crate = _get_bits(hd, 16, 4)
    cratetype = _get_bits(hd, 20, 4)
    marker = _get_bits(hd, 24, 3)
    geo = _get_bits(hd, 27, 5)
    valid = 0 if marker == 2 else 254 if marker == 6 else 1
    if verb:
        print("data header 0x%x geo %d marker %d crate %d type %d zero %d chans %d not used %d - valid %s" % (hd, geo, marker, crate, cratetype, zero, nch, u, str(valid) if valid else 'ok'))
    return valid, {"u": u, "n": nch, "z": zero, "c": crate, "t": cratetype, "m": marker, "g": geo, "raw": hd}

def parse_trail(tr, verb = False):
    tr = _u32(tr)
    c = _get_bits(tr, 0, 24)
    marker = _get_bits(tr, 24, 3)
    geo = _get_bits(tr, 27, 5)
    valid = 0 if marker == 4 else 2
    if verb:
        print("data trail 0x%x geo %d marker %d event counter %d - valid %s" % (tr, geo, marker, c, str(valid) if valid else 'ok'))
    return valid, {"c": c, "m": marker, "g": geo, "raw": tr}

def parse_data(dt, spec = 0, verb = False): # spec from header[20:23]

    dt = _u32(dt)
    v = _get_bits(dt, 0, 12)
    f = _get_bits(dt, 12, 3)
    u = _get_bits(dt, 15, 1)  # "unused"
    c = _get_bits(dt, 16, 8)
    m = _get_bits(dt, 24, 3)
    g = _get_bits(dt, 27, 5)

    
    if spec == 0b1010:  # QDC V792 32 channels
        chan = c & 0b11111
        flags = f & 0b11
        valid = 0 if m == 0 and flags == 0 else 2*10+flags
        if verb:
            print("QDC index %d data 0x%x geo %d marker %d chan %02d not used %d flags %d val %d - valid %s" % (spec, dt, g, m, chan, u, flags, v, str(valid) if valid else 'ok'))

    elif spec == 0b1001: # QDC V792N 16 channels
        chan = (c >> 1) & 0b1111
        flags = f & 0b11
        valid = 0 if m == 0 and flags == 0 else 3*10+flags
        if verb:
            print("QDC index %d data 0x%x geo %d marker %d chan %02d not used %d flags %d val %d - valid %s" % (spec, dt, g, m, chan, u, flags, v, str(valid) if valid else 'ok'))

    elif spec == 0b0110: # TDC V775 32 channels
        chan = c & 0b11111
        flags = f & 0b11
        vbit = (f >> 2) & 0b1
        valid = 0 if m == 0 and vbit == 1 and flags == 0 else 4*10+flags
        if verb:
            print("TDC index %d data 0x%x geo %d marker %d chan %02d not used %d valbit %d flags %d val %d - valid %s" % (spec, dt, g, m, chan, u, vbit, flags, v, str(valid) if valid else 'ok'))

    elif spec == 0b0101: # TDC V775N 16 channels
        chan = (c >> 1) & 0b1111
        flags = f & 0b11
        vbit = (f >> 2) & 0b1
        valid = 0 if m == 0 and vbit == 1 and flags == 0 else 5*10+flags
        if verb:
            print("TDC index %d data 0x%x geo %d marker %d chan %02d not used %d valbit %d flags %d val %d - valid %s" % (spec, dt, g, m, chan, u, vbit, flags, v, str(valid) if valid else 'ok'))
        
    else: # Corrupted header
        return 99, {}
        
    return valid, {"v": v, "f": flags, "u": u, "c": chan, "m": m, "g": g, "raw": dt}




def decodeblock(line, verb = False): # line is a single string for one event
    """Decode  full event. 
    Error in data header --> stop decoding and forward
    Error in data trailer --> stop decoding and forward
    Error in data payload --> do not stop decoding. TDC sanity flags stored in output
    """

    if verb:
        print(line)
    block = [int(i,16) for i in line.split()]

    valid = []
    ADC = {} # ADC[channel] = value
    TDC = {} # TDC[channel] = (value, flag) <-- non-zero flag for OV or UN

    adcset = set()
    tdcset = set()

    INDEX = 0
    nDataHeader = 0

    v, HEAD = parse_evt_header(block[0:14], verb=verb)
    if v:
        valid.append(v)
        return valid, HEAD, ADC, TDC
    INDEX = 14

    PayloadSize = HEAD["payloadsize"]
    
    #while INDEX < len(block)-1:
    while INDEX < 14 + PayloadSize:
        v, w = parse_head(block[INDEX], verb=verb)
        INDEX += 1
        crate = w["c"]
        cratetype = w["t"]
        isQDC = bool((cratetype >> 3) & 0b1)
        if v == 254 and INDEX == 14 + PayloadSize:  # 0xFE... words are tolerated at the end of the event
            return valid, HEAD, ADC, TDC
        elif v:
            valid.append(v)
            return valid, HEAD, ADC, TDC

        nDataHeader += 1
        for iword in range(w["n"]):

            if INDEX == len(block):
                valid.append(74)
                return valid, HEAD, ADC, TDC
            
            v, w = parse_data(block[INDEX], spec = cratetype, verb=verb)
            INDEX += 1
            if v:
                valid.append(v)

            if isQDC:
                chan = crate*32 + w["c"]
            else: # Aassuming only one TDC module 
                chan = w["c"]
            
            if isQDC:
                if chan in adcset:
                    valid.append(111)
                adcset.add(chan)
                ADC[chan] = w["v"]
            else:
                if chan in tdcset:
                    valid.append(112)
                tdcset.add(chan)
                TDC[chan] = (w["v"], w["f"])

        v, w = parse_trail(block[INDEX], verb=verb)
        INDEX += 1
        if v:
            valid.append(v)
            return valid, HEAD, ADC, TDC

    # if INDEX != DATA SINZE WRITTEN IN HEADER
    if INDEX >= len(block):
        valid.append(74)
        return valid, HEAD, ADC, TDC
    v, w = parse_evt_trail(block[INDEX], verb=verb)
    INDEX += 1
    if INDEX != len(block):
        valid.append(75)
        return valid, HEAD, ADC, TDC
    if v:
        valid.append(v)

    return valid, HEAD, ADC, TDC

        

    

# ---- Optional: small demo when run directly ---------------------------------
#if __name__ == "__main__":
#    
#    
#    import sys
#    if len(sys.argv) != 2:
#        print("Usage: python2 %s <file>" % sys.argv[0])
#        sys.exit(1)
#
#    path = sys.argv[1]
#
#    
#    with open(path, "r") as f:
#        evt = 0
#        for line in f:
#            line = line.strip()   # remove leading/trailing whitespace & newline
#            print("--- Evt %d ---" %evt)
#            evt += 1
#            valid, head, adc, tdc = decodeblock(line)
#            for v in valid:
#                print("DECODING ERROR code %d : %s" %(v, DecErr[v]))
#                print(adc)
#                print(tdc)

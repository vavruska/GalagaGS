        case on
        mcopy o:mouse.macros
MOUSE_STATUS    gequ $c027
MOUSE_DATA_REG  gequ $c024

mouseAsm        start
        using mouseData
mousePoll    entry
                ldx #0
                ldy #0
                short i,m
                phb
                lda #$e0
                pha
                plb
                lda MOUSE_STATUS
                bpl   noMouse
                and  #2
                bne  mouseYOnly
                ldx   MOUSE_DATA_REG
mouseYOnly  anop
                ldy   MOUSE_DATA_REG
donexy   anop
                phk
                plb
                long i,m
                stx   deltaX
                sty   deltaY
                txa
                and #$0080
                beq noBut
                tya
                and #$0080
                beq noBut
                stz mouseBut
                bra done
noBut anop
                lda #1
                sta mouseBut
done anop
                plb
                rtl
noMouse anop
                long i,m
                phk
                plb
                stz deltaX
                stz deltaY
                plb
                rtl
        end

mouseData  data
deltaX              entry
                        dc i2'0'
deltaY              entry
                        dc i2'0'
mouseBut        entry
                         dc i2'0'
                end

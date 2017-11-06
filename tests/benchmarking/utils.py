import os, copy, io
from collections import OrderedDict

def try_float( sVal, dft=None ):
    res = dft
    try:
        res = float( sVal )
    except:
        pass
    return res

def mergeDict( dict1, dict2 ):
    for key in dict2:
        dict1[ key ] = dict2[ key ]

def addMapValues( dict1, dict2 ):
    for key in dict2:
        if key in dict1:
            dict1[key] += dict2[key]
        else:
            dict1[key] = dict2[key]
            
class MatrixByStr:          ### Matrix with string keys
    def __init__( self,
            rows, cols,          ## each a list of tuples: (short, long) row/column names
            defval = 0 ):
        self.rows, self.cols = rows, cols
        self.mbs = OrderedDict( (r[0],
            OrderedDict( (c[0], copy.deepcopy(defval)) for c in cols )) for r in rows )

    def __getitem__( self, k ):
        return self.mbs[ k[0] ][ k[1] ]

    def __setitem__( self, k, v ):
        self.mbs[ k[0] ][ k[1] ] = v

    def stringify2D( self ):           ## compact 2D print
        return MyTab().tabulate( [
            [key] + list(row.values())        ## merge each row's key into the row
              for key,row in self.mbs.items() ],
            [""] + [ col[0] for col in self.cols ] )

    ## assumes each item is a list
    def stringifyLists( self, title, rowIntro, colIntro ):
        resIO = io.StringIO()
        print( '#'*50, file=resIO )
        print( '#'*50, file=resIO )
        print( '#'*50, file=resIO )
        print( title, file=resIO )
        print( '#'*50, file=resIO )
        print( '#'*50, file=resIO )
        print( '#'*50, file=resIO )
        print( file=resIO )

        for kr, r in self.mbs.items():
            print( '#'*50, file=resIO )
            print( '#'*50, file=resIO )
            print( rowIntro, kr, file=resIO )
            print( '#'*50, file=resIO )
            print( '#'*50, file=resIO )
            print( file=resIO )

            for kc, c in r.items():
                print( '#'*50, file=resIO )
                print( rowIntro, kr, file=resIO )
                print( "   ", colIntro, kc, file=resIO )
                print( '#'*50, file=resIO )

                for el in c:
                    print( el, file=resIO )
                print( file=resIO )

        return resIO.getvalue()

class MyTab:
    def __init__( self ):
        self.sColSep='  '
      
    ## Receive table row-wise
    def tabulate( self, matr, hdr ):
        res = ""
        ## Compute max width
        nWMax = []
        for iC in range( len( hdr ) ):
            nWMax.append( len( str( hdr[ iC ] ) ) )
            for iR in range( len( matr ) ):
                if iC<len( matr[ iR ] ):
                    if nWMax[ iC ]<len( str( matr[iR][iC] ) ):
                        nWMax[ iC ] = len( str( matr[iR][iC] ) )
        ## Printing
        res =( ("{0:<"+str(nWMax[0])+'}').format(hdr[0]) + self.sColSep )
        for iC in range( 1, len( hdr ) ):
            res += ( ("{0:"+str(nWMax[iC])+'}').format(hdr[iC])
                  + self.sColSep if iC+1<len(hdr) else '\n' )
        for iR in range( len( matr ) ):
            res += ( ("{0:<"+str(nWMax[0])+'}').format(matr[iR][0] if 0<len( matr[ iR ] ) else '-') + self.sColSep )
            for iC in range( 1, len( matr[iR] ) ):
                res += ( ("{0:"+str(nWMax[iC])+'}').format(matr[iR][iC] if iC<len( matr[ iR ] ) else '-')
                       + self.sColSep if iC+1<len(hdr) else '\n' )
        return res

## change string to be abke to become a filename
def flnfy( sStr ):
    keepcharacters = ('-','_')
    return "".join(c if c.isalnum() or c in keepcharacters else 'I' for c in sStr).strip()

def makeDirname( sFln ):
    sDir = os.path.dirname( sFln )
    if 0<len(sDir):
        try:
            os.makedirs( sDir, exist_ok=True)
        except:
            print( "Failed to create dir '", sDir, "'.", sep='' )
            return False
    return True

def openFile_autoDir( sFln, sMode ):
    makeDirname( sFln )
    return open( sFln, sMode )

import os

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
        print( ("{0:<"+str(nWMax[0])+'}').format(hdr[0]), end=self.sColSep )
        for iC in range( 1, len( hdr ) ):
            print( ("{0:"+str(nWMax[iC])+'}').format(hdr[iC]),
                  end=self.sColSep if iC+1<len(hdr) else '\n' )
        for iR in range( len( matr ) ):
            print( ("{0:<"+str(nWMax[0])+'}').format(matr[iR][0] if 0<len( matr[ iR ] ) else '-'), end=self.sColSep )
            for iC in range( 1, len( matr[iR] ) ):
                print( ("{0:"+str(nWMax[iC])+'}').format(matr[iR][iC] if iC<len( matr[ iR ] ) else '-'),
                      end=self.sColSep if iC+1<len(hdr) else '\n' )
        return res

def openFile_autoDir( sFln, sMode ):
    sDir = os.path.dirname( sFln )
    if 0<len(sDir):
        try:
            os.makedirs( sDir, exist_ok=True)
        except:
            print( "Failed to create dir '", sDir, "'.", sep='' )
    return open( sFln, sMode )

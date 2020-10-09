import json, copy

                ######################## Methods to store program config in JSON ########
                ######################## Config can be incrementable, see mergeJSON #####
                ########################### JSON CONFIG & STUFF #########################
n_JSON_Indent = 4
s_AddKey = "ADD_KEY::"          ## The prefix for keys to be added in new backend definitions
                                ## The prefix itself is not added
                                ## TODO: remove in lvalue
s_CommentKey = "///COMMENT"     ## The key for comments in JSON config dictionaries

## Returns a merge of j2 into j1.
def mergeJSON( j1, j2 ):
    jNew = None
    if dict==type(j1) and dict==type(j2):
        jNew = copy.deepcopy( j1, {} )
        for key in j2:
            if key in j1:
                jNew[key] = mergeJSON( j1[key], j2[key] )
            else:
                assert key.startswith( s_AddKey ), "Adding key %s: does not start with the prefix %s, j1: %s" % (key, s_AddKey, j1.__str__())
                jNew[key[len(s_AddKey):]] = j2[key]
    elif list==type(j1) and list==type(j2):
        jNew = []
        for i in range( 0, len(j2) ):     ## Can assign a shorter list this way
            if i<len(j1):
                jNew.append( mergeJSON( j1[i], j2[i] ) )
            else:
                jNew.append( j2[i] )
    elif type(j1)==type(j2):
        jNew = j2
    else:
        print( "\n\n ERROR: Could not merge JSON objects:" );
        print( json.dumps( j1 ) )
        print( "\n -- TO BE MERGED BY --\n" )
        print( json.dumps( j2 ) )
        sys.quit()
    return jNew


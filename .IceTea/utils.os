utils = extends _G {
    sourceTree: function(base, sources) {
        for(var i,v in sources) {
            sources[i] = pfs.join(base, v).sub(0,-1);
        }
        return sources;
    }
}

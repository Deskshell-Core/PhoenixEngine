utils = extends _G {
    sourceTree: function(base, sources) {
        for(var i,v in sources) {
            sources[i] = pfs.join(base, sources[i]);
        }
        return sources;
    }
}

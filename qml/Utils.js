.pragma library

function int2(i) {
    return i < 10 ? ("0" + i) : i
}

function formatPlayTime(secs) {
    if (secs >= 3600) {
        var h = Math.floor(secs / 3600)
        var m = Math.floor((secs % 3600) / 60)
        return h + ":" + int2(m) + ":" + int2(secs % 60)
    } else if (secs >= 60) {
        var min = Math.floor(secs / 60)
        return min + ":" + int2(secs % 60)
    } else {
        return "0:" + int2(secs % 60)
    }
}

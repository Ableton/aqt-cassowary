.pragma library

"use strict"

function optional(obj, prop, def) {
    var r = obj[prop];
    return r == null ? def : r;
}

function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value))
}

function getItems(items) {
    var res = []
    forEachItem(items, function (x) {
        res.push(x);
    })
    return res
}

function closest(coll, value) {
    return coll && coll.reduce(function (s, v) {
        var diff = Math.abs(value - v)
        return diff < s.best ? { best: diff, value: v } : s
    }, {
        best: Infinity
    }).value
}

function forEachItem(items, fn) {
    var count = items.count
    for (var i = 0; i < count; ++i) {
        fn(items.itemAt(i), i)
    }
}

function almost(value, other, err) {
    err = err || 0.0001;
    return Math.abs(value - other) < err;
}

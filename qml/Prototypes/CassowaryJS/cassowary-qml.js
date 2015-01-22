.pragma library

/**
 * Parts Copyright (C) 2011-2012, Alex Russell (slightlyoff@chromium.org)
 * Parts Copyright (C) Copyright (C) 1998-2000 Greg J. Badros
 *
 * Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
 *
 * This is a compiled version of Cassowary/JS. For source versions or to
 * contribute, see the github project:
 *
 *  https://github.com/slightlyoff/cassowary-js-refactor
 *
 */

var cassowaryScope = {};

(function() {
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by
    //
    //    http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011-2012, Alex Russell (slightlyoff@chromium.org)

    (function(scope){
        "use strict";

        // For Safari 5.x. Go-go-gadget ridiculously long release cycle!
        try {
            (function(){}).bind(scope);
        } catch (e) {
            Object.defineProperty(Function.prototype, "bind", {
                value: function(scope) {
                    var f = this;
                    return function() { return f.apply(scope, arguments); }
                },
                enumerable: false,
                configurable: true,
                writable: true,
            });
        }

        var inBrowser = (typeof scope["HTMLElement"] != "undefined");

        var getTagName = function(proto) {
            var tn = null;
            while (proto && proto != Object.prototype) {
                if (proto.tagName) {
                    tn = proto.tagName;
                    break;
                }
                proto = proto.prototype;
            }
            return tn || "div";
        };
        var epsilon = 1e-8;

        var  _t_map = {};
        var walkForMethod = function(ctor, name) {
            if (!ctor || !name) return;

            // Check the class-side first, the look at the prototype, then walk up
            if (typeof ctor[name] == "function") {
                return ctor[name];
            }
            var p = ctor.prototype;
            if (p && typeof p[name] == "function") {
                return p[name];
            }
            if (p === Object.prototype ||
                p === Function.prototype) {
                return;
            }

            if (typeof ctor.__super__ == "function") {
                return walkForMethod(ctor.__super__, name);
            }
        };

        // Global
        var c = scope.c = function() {
            if(c._api) {
                return c._api.apply(this, arguments);
            }
        };

        //
        // Configuration
        //
        c.debug = false;
        c.trace = false;
        c.verbose = false;
        c.traceAdded = false;
        c.GC = false;

        //
        // Constants
        //
        c.GEQ = 1;
        c.LEQ = 2;


        //
        // Utility methods
        //
        c.inherit = function(props) {
            var ctor = null;
            var parent = null;

            if (props["extends"]) {
                parent = props["extends"];
                delete props["extends"];
            }

            if (props["initialize"]) {
                ctor = props["initialize"];
                delete props["initialize"];
            }

            var realCtor = ctor || function() { };

            Object.defineProperty(realCtor, "__super__", {
                value: (parent) ? parent : Object,
                enumerable: false,
                configurable: true,
                writable: false,
            });

            if (props["_t"]) {
                _t_map[props["_t"]] = realCtor;
            }

            // FIXME(slightlyoff): would like to have class-side inheritance!
            // It's easy enough to do when we have __proto__, but we don't in IE 9/10.
            //   = (

            /*
            // NOTE: would happily do this except it's 2x slower. Boo!
            props.__proto__ = parent ? parent.prototype : Object.prototype;
            realCtor.prototype = props;
            */

            var rp = realCtor.prototype = Object.create(
                ((parent) ? parent.prototype : Object.prototype)
            );

            c.extend(rp, props);

            // If we're in a browser, we want to support "subclassing" HTML elements.
            // This needs some magic and we rely on a wrapped constructor hack to make
            // it happen.
            if (inBrowser) {
                if (parent && parent.prototype instanceof scope.HTMLElement) {
                    var intermediateCtor = realCtor;
                    var tn = getTagName(rp);
                    var upgrade = function(el) {
                        el.__proto__ = rp;
                        intermediateCtor.apply(el, arguments);
                        if (rp["created"]) { el.created(); }
                        if (rp["decorate"]) { el.decorate(); }
                        return el;
                    };
                    this.extend(rp, { upgrade: upgrade, });

                    realCtor = function() {
                        // We hack the constructor to always return an element with it's
                        // prototype wired to ours. Boo.
                        return upgrade(
                            scope.document.createElement(tn)
                        );
                    }
                    realCtor.prototype = rp;
                    this.extend(realCtor, { ctor: intermediateCtor, }); // HACK!!!
                }
            }

            return realCtor;
        };

        c.own = function(obj, cb, context) {
            Object.getOwnPropertyNames(obj).forEach(cb, context||scope);
            return obj;
        };

        c.extend = function(obj, props) {
            c.own(props, function(x) {
                var pd = Object.getOwnPropertyDescriptor(props, x);
                try {
                    if ( (typeof pd["get"] == "function") ||
                         (typeof pd["set"] == "function") ) {
                        Object.defineProperty(obj, x, pd);
                    } else if (typeof pd["value"] == "function" ||x.charAt(0) === "_") {
                        pd.writable = true;
                        pd.configurable = true;
                        pd.enumerable = false;
                        Object.defineProperty(obj, x, pd);
                    } else {
                        obj[x] = props[x];
                    }
                } catch(e) {
                    // console.warn("c.extend assignment failed on property", x);
                }
            });
            return obj;
        };

        // FIXME: legacy API to be removed
        c.traceprint = function(s /*String*/) { if (c.verbose) { console.log(s); } };
        c.fnenterprint = function(s /*String*/) { console.log("* " + s); };
        c.fnexitprint = function(s /*String*/) { console.log("- " + s); };

        c.assert = function(f /*boolean*/, description /*String*/) {
            if (!f) {
                throw new c.InternalError("Assertion failed: " + description);
            }
        };

        var exprFromVarOrValue = function(v) {
            if (typeof v == "number" ) {
                return c.Expression.fromConstant(v);
            } else if(v instanceof c.Variable) {
                return c.Expression.fromVariable(v);
            }
            return v;
        };

        c.plus = function(e1, e2) {
            e1 = exprFromVarOrValue(e1);
            e2 = exprFromVarOrValue(e2);
            return e1.plus(e2);
        };

        c.minus = function(e1, e2) {
            e1 = exprFromVarOrValue(e1);
            e2 = exprFromVarOrValue(e2);
            return e1.minus(e2);
        };

        c.times = function(e1, e2) {
            e1 = exprFromVarOrValue(e1);
            e2 = exprFromVarOrValue(e2);
            return e1.times(e2);
        };

        c.divide = function(e1, e2) {
            e1 = exprFromVarOrValue(e1);
            e2 = exprFromVarOrValue(e2);
            return e1.divide(e2);
        };

        c.approx = function(a, b) {
            if (a === b) { return true; }
            a = +(a);
            b = +(b);
            if (a == 0) {
                return (Math.abs(b) < epsilon);
            }
            if (b == 0) {
                return (Math.abs(a) < epsilon);
            }
            return (Math.abs(a - b) < Math.abs(a) * epsilon);
        };

        var count = 1;
        c._inc = function() { return count++; };

        c.parseJSON = function(str) {
            return JSON.parse(str, function(k, v) {
                if (typeof v != "object" || typeof v["_t"] != "string") {
                    return v;
                }
                var type = v["_t"];
                var ctor = _t_map[type];
                if (type && ctor) {
                    var fromJSON = walkForMethod(ctor, "fromJSON");
                    if (fromJSON) {
                        return fromJSON(v, ctor);
                    }
                }
                return v;
            });
        };

        if (typeof define === 'function' && define.amd) {
            // Require.js
            define(c);
        } else if (typeof module === 'object' && module.exports) {
            // CommonJS
            module.exports = c;
        } else {
            // Browser without module container
            scope.c = c;
        }

    })(this);
    /**
     * Copyright 2012 Alex Russell <slightlyoff@google.com>.
     *
     * Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
     *
     * This is an API compatible re-implementation of the subset of jshashtable
     * which Cassowary actually uses.
     *
     * Features removed:
     *
     *     - multiple values per key
     *     - error tollerent hashing of any variety
     *     - overly careful (or lazy) size counting, etc.
     *     - Crockford's "class" pattern. We use the system from c.js.
     *     - any attempt at back-compat with broken runtimes.
     *
     * APIs removed, mostly for lack of use in Cassowary:
     *
     *     - support for custom hashing and equality functions as keys to ctor
     *     - isEmpty() -> check for !ht.size()
     *     - putAll()
     *     - entries()
     *     - containsKey()
     *     - containsValue()
     *     - keys()
     *     - values()
     *
     * Additions:
     *
     *     - new "scope" parameter to each() and escapingEach()
     */

    (function(c) {
        "use strict";

        var copyOwn = function(src, dest) {
            Object.keys(src).forEach(function(x) {
                dest[x] = src[x];
            });
        };

        if (false && typeof Map != "undefined") {

            c.HashTable = c.inherit({

                initialize: function() {
                    this.size = 0;
                    this._store = new Map();
                    this._keys = [];
                    // this.get = this._store.get.bind(this._store);
                },

                set: function(key, value) {
                    this._store.set(key, value);
                    if (this._keys.indexOf(key) == -1) {
                        this.size++;
                        // delete this._keys[this._keys.indexOf(key)];
                        this._keys.push(key);
                    } /* else {
                         delete this._keys[this._keys.indexOf(key)];
                         this._keys.push(key);
                         }
                      */
                },

                get: function(key) {
                    return this._store.get(key);
                },

                clear: function() {
                    this.size = 0;
                    this._store = new Map();
                    this._keys = [];
                },

                delete: function(key) {
                    if (this._store.delete(key) && this.size > 0) {
                        delete this._keys[this._keys.indexOf(key)];
                        this.size--;
                    }
                },

                each: function(callback, scope) {
                    if (!this.size) { return; }
                    this._keys.forEach(function(k){
                        if (typeof k == "undefined") { return; }
                        var v = this._store.get(k);
                        if (typeof v != "undefined") {
                            callback.call(scope||null, k, v);
                        }
                    }, this);
                },

                escapingEach: function(callback, scope) {
                    if (!this.size) { return; }

                    var that = this;
                    var kl = this._keys.length;
                    var context;
                    for (var x = 0; x < kl; x++) {
                        if (typeof this._keys[x] != "undefined") {
                            (function(k) {
                                var v = that._store.get(k);
                                if (typeof v != "undefined") {
                                    context = callback.call(scope||null, k, v);
                                }
                            })(this._keys[x]);

                            if (context) {
                                if (context.retval !== undefined) {
                                    return context;
                                }
                                if (context.brk) {
                                    break;
                                }
                            }
                        }
                    }
                },

                clone: function() {
                    var n = new c.HashTable();
                    if (this.size) {
                        this.each(function(k, v) {
                            n.set(k, v);
                        });
                    }
                    return n;
                }
            });
        } else {
            // For escapingEach
            var defaultContext = {};

            c.HashTable = c.inherit({

                initialize: function() {
                    this.size = 0;
                    this._store = {};
                    this._keyStrMap = {};
                    this._deleted = 0;
                },

                set: function(key, value) {
                    var hash = key.hashCode;

                    if (typeof this._store[hash] == "undefined") {
                        // FIXME(slightlyoff): if size gooes above the V8 property limit,
                        // compact or go to a tree.
                        this.size++;
                    }
                    this._store[hash] = value;
                    this._keyStrMap[hash] = key;
                },

                get: function(key) {
                    if(!this.size) { return null; }

                    key = key.hashCode;

                    var v = this._store[key];
                    if (typeof v != "undefined") {
                        return this._store[key];
                    }
                    return null;
                },

                clear: function() {
                    this.size = 0;
                    this._store = {};
                    this._keyStrMap = {};
                },

                _compact: function() {
                    // console.time("HashTable::_compact()");
                    var ns = {};
                    copyOwn(this._store, ns);
                    this._store = ns;
                    // console.timeEnd("HashTable::_compact()");
                },

                _compactThreshold: 100,
                _perhapsCompact: function() {
                    // If we have more properties than V8's fast property lookup limit, don't
                    // bother
                    if (this._size > 30) return;
                    if (this._deleted > this._compactThreshold) {
                        this._compact();
                        this._deleted = 0;
                    }
                },

                delete: function(key) {
                    key = key.hashCode;
                    if (!this._store.hasOwnProperty(key)) {
                        return;
                    }
                    this._deleted++;

                    // FIXME(slightlyoff):
                    //    I hate this because it causes these objects to go megamorphic = (
                    //    Sadly, Cassowary is hugely sensitive to iteration order changes, and
                    //    "delete" preserves order when Object.keys() is called later.
                    delete this._store[key];
                    // Note: we don't delete from _keyStrMap because we only get the
                    // Object.keys() from _store, so it's the only one we need to keep up-to-
                    // date.

                    if (this.size > 0) {
                        this.size--;
                    }
                },

                each: function(callback, scope) {
                    if (!this.size) { return; }

                    this._perhapsCompact();

                    var store = this._store;
                    var keyMap = this._keyStrMap;
                    for (var x in this._store) {
                        if (this._store.hasOwnProperty(x)) {
                            callback.call(scope||null, keyMap[x], store[x]);
                        }
                    }
                },

                escapingEach: function(callback, scope) {
                    if (!this.size) { return; }

                    this._perhapsCompact();

                    var that = this;
                    var store = this._store;
                    var keyMap = this._keyStrMap;
                    var context = defaultContext;
                    var kl = Object.keys(store);
                    for (var x = 0; x < kl.length; x++) {
                        (function(v) {
                            if (that._store.hasOwnProperty(v)) {
                                context = callback.call(scope||null, keyMap[v], store[v]);
                            }
                        })(kl[x]);

                        if (context) {
                            if (context.retval !== undefined) {
                                return context;
                            }
                            if (context.brk) {
                                break;
                            }
                        }
                    }
                },

                clone: function() {
                    var n = new c.HashTable();
                    if (this.size) {
                        n.size = this.size;
                        copyOwn(this._store, n._store);
                        copyOwn(this._keyStrMap, n._keyStrMap);
                    }
                    return n;
                },

                equals: function(other) {
                    if (other === this) {
                        return true;
                    }

                    if (!(other instanceof c.HashTable) || other._size !== this._size) {
                        return false;
                    }

                    var codes = Object.keys(this._store);
                    for (var i = 0; i < codes.length; i++) {
                        var code = codes[i];
                        if (this._keyStrMap[code] !== other._keyStrMap[code] ||
                            this._store[code] !== other._store[code]) {
                            return false;
                        }
                    }

                    return true;
                },

                toString: function(h) {
                    var answer = "";
                    this.each(function(k, v) { answer += k + " => " + v + "\n"; });
                    return answer;
                },
            });
        }

    })(this["c"]||module.parent.exports||{});
    /**
     * Copyright 2011, Alex Russell <slightlyoff@google.com>
     *
     * Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
     *
     * API compatible re-implementation of jshashset.js, including only what
     * Cassowary needs. Built for speed, not comfort.
     */
    (function(c) {
        "use strict";

        c.HashSet = c.inherit({
            _t: "c.HashSet",

            initialize: function() {
                this.storage = [];
                this.size = 0;
                this.hashCode = c._inc();
            },

            add: function(item) {
                var s = this.storage, io = s.indexOf(item);
                if (s.indexOf(item) == -1) { s[s.length] = item; }
                this.size = this.storage.length;
            },

            values: function() {
                // FIXME(slightlyoff): is it safe to assume we won't be mutated by our caller?
                //                     if not, return this.storage.slice(0);
                return this.storage;
            },

            has: function(item) {
                var s = this.storage;
                return (s.indexOf(item) != -1);
            },

            delete: function(item) {
                var io = this.storage.indexOf(item);
                if (io == -1) { return null; }
                this.storage.splice(io, 1)[0];
                this.size = this.storage.length;
            },

            clear: function() {
                this.storage.length = 0;
            },

            each: function(func, scope) {
                if(this.size)
                    this.storage.forEach(func, scope);
            },

            escapingEach: function(func, scope) {
                // FIXME(slightlyoff): actually escape!
                if (this.size)
                    this.storage.forEach(func, scope);
            },

            toString: function() {
                var answer = this.size + " {";
                var first = true;
                this.each(function(e) {
                    if (!first) {
                        answer += ", ";
                    } else {
                        first = false;
                    }
                    answer += e;
                });
                answer += "}\n";
                return answer;
            },

            toJSON: function() {
                var d = [];
                this.each(function(e) {
                    d[d.length] = e.toJSON();
                });
                return {
                    _t: "c.HashSet",
                    data: d
                };
            },

            fromJSON: function(o) {
                var r = new c.HashSet();
                if (o.data) {
                    r.size = o.data.length;
                    r.storage = o.data;
                }
                return r;
            },
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011-2012, Alex Russell (slightlyoff@chromium.org)

    (function(c){
        "use strict";

        c.Error = c.inherit({
            // extends: Error,
            initialize: function(s /*String*/) { if (s) { this._description = s; } },
            _name: "c.Error",
            _description: "An error has occured in Cassowary",
            set description(v)   { this._description = v; },
            get description()    { return "(" + this._name + ") " + this._description; },
            get message()        { return this.description; },
            toString: function() { return this.description; },
        });

        var errorType = function(name, error) {
            return c.inherit({
                extends: c.Error,
                initialize: function() { c.Error.apply(this, arguments); },
                _name: name||"", _description: error||""
            });
        };

        c.ConstraintNotFound =
            errorType("c.ConstraintNotFound",
                      "Tried to remove a constraint never added to the tableu");

        c.InternalError =
            errorType("c.InternalError");

        c.NonExpression =
            errorType("c.NonExpression",
                      "The resulting expression would be non");

        c.NotEnoughStays =
            errorType("c.NotEnoughStays",
                      "There are not enough stays to give specific values to every variable");

        c.RequiredFailure =
            errorType("c.RequiredFailure", "A required constraint cannot be satisfied");

        c.TooDifficult =
            errorType("c.TooDifficult", "The constraints are too difficult to solve");

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011-2012, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";

        var multiplier = 1000;

        c.SymbolicWeight = c.inherit({
            _t: "c.SymbolicWeight",
            initialize: function(/*w1, w2, w3*/) {
                this.value = 0;
                var factor = 1;
                for (var i = arguments.length - 1; i >= 0; --i) {
                    this.value += arguments[i] * factor;
                    factor *= multiplier;
                }
            },

            toJSON: function() {
                return {
                    _t: this._t,
                    value: this.value
                };
            },
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    // FILE: EDU.Washington.grad.gjb.cassowary
    // package EDU.Washington.grad.gjb.cassowary;

    (function(c) {

        c.Strength = c.inherit({
            initialize: function(name /*String*/, symbolicWeight, w2, w3) {
                this.name = name;
                if (symbolicWeight instanceof c.SymbolicWeight) {
                    this.symbolicWeight = symbolicWeight;
                } else {
                    this.symbolicWeight = new c.SymbolicWeight(symbolicWeight, w2, w3);
                }
            },

            get required() {
                return (this === c.Strength.required);
            },

            toString: function() {
                return this.name + (!this.isRequired ? (":" + this.symbolicWeight) : "");
            },
        });

        /* public static final */
        c.Strength.required = new c.Strength("<Required>", 1000, 1000, 1000);
        /* public static final  */
        c.Strength.strong = new c.Strength("strong", 1, 0, 0);
        /* public static final  */
        c.Strength.medium = new c.Strength("medium", 0, 1, 0);
        /* public static final  */
        c.Strength.weak = new c.Strength("weak", 0, 0, 1);

    })(this["c"]||((typeof module != "undefined") ? module.parent.exports.c : {}));
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011-2012, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";

        c.AbstractVariable = c.inherit({
            isDummy:      false,
            isExternal:   false,
            isPivotable:  false,
            isRestricted: false,

            _init: function(args, varNamePrefix) {
                // Common mixin initialization.
                this.hashCode = c._inc();
                this.name = (varNamePrefix||"") + this.hashCode;
                if (args) {
                    if (typeof args.name != "undefined") {
                        this.name = args.name;
                    }
                    if (typeof args.value != "undefined") {
                        this.value = args.value;
                    }
                    if (typeof args.prefix != "undefined") {
                        this._prefix = args.prefix;
                    }
                }
            },

            _prefix: "",
            name: "",
            value: 0,

            valueOf: function() { return this.value; },

            toJSON: function() {
                var o = {};
                if (this._t) {
                    o._t = this._t;
                }
                if (this.name) {
                    o.name = this.name;
                }
                if (typeof this.value != "undefined") {
                    o.value = this.value;
                }
                if (this._prefix) {
                    o._prefix = this._prefix;
                }
                if (this._t) {
                    o._t = this._t;
                }
                return o;
            },

            fromJSON: function(o, Ctor) {
                var r = new Ctor();
                c.extend(r, o);
                return r;
            },

            toString: function() {
                return this._prefix + "[" + this.name + ":" + this.value + "]";
            },

        });

        c.Variable = c.inherit({
            _t: "c.Variable",
            extends: c.AbstractVariable,
            initialize: function(args) {
                this._init(args, "v");
                var vm = c.Variable._map;
                if (vm) { vm[this.name] = this; }
            },
            isExternal:     true,
        });

        /* static */
        // c.Variable._map = [];

        c.DummyVariable = c.inherit({
            _t: "c.DummyVariable",
            extends: c.AbstractVariable,
            initialize: function(args) {
                this._init(args, "d");
            },
            isDummy:        true,
            isRestricted:   true,
            value:         "dummy",
        });

        c.ObjectiveVariable = c.inherit({
            _t: "c.ObjectiveVariable",
            extends: c.AbstractVariable,
            initialize: function(args) {
                this._init(args, "o");
            },
            value:         "obj",
        });

        c.SlackVariable = c.inherit({
            _t: "c.SlackVariable",
            extends: c.AbstractVariable,
            initialize: function(args) {
                this._init(args, "s");
            },
            isPivotable:    true,
            isRestricted:   true,
            value:         "slack",
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";
        c.Point = c.inherit({
            initialize: function(x, y, suffix) {
                if (x instanceof c.Variable) {
                    this._x = x;
                } else {
                    var xArgs = { value: x };
                    if (suffix) {
                        xArgs.name = "x" + suffix;
                    }
                    this._x = new c.Variable(xArgs);
                }
                if (y instanceof c.Variable) {
                    this._y = y;
                } else {
                    var yArgs = { value: y };
                    if (suffix) {
                        yArgs.name = "y" + suffix;
                    }
                    this._y = new c.Variable(yArgs);
                }
            },

            get x() { return this._x; },
            set x(xVar) {
                if (xVar instanceof c.Variable) {
                    this._x = xVar;
                } else {
                    this._x.value = xVar;
                }
            },

            get y() { return this._y; },
            set y(yVar) {
                if (yVar instanceof c.Variable) {
                    this._y = yVar;
                } else {
                    this._y.value = yVar;
                }
            },

            toString: function() {
                return "(" + this.x + ", " + this.y + ")";
            },
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    // FILE: EDU.Washington.grad.gjb.cassowary
    // package EDU.Washington.grad.gjb.cassowary;

    (function(c) {
        "use strict";

        var checkNumber = function(value, otherwise){
            // if(isNaN(value)) { debugger; }
            return (typeof value === "number") ? value : otherwise;
        };

        c.Expression = c.inherit({

            initialize: function(cvar /*c.AbstractVariable*/,
                                 value /*double*/,
                                 constant /*double*/) {
                this.constant = checkNumber(constant, 0);
                this.terms = new c.HashTable();
                if (cvar instanceof c.AbstractVariable) {
                    value = checkNumber(value, 1);
                    this.setVariable(cvar, value);
                } else if (typeof cvar == "number") {
                    if (!isNaN(cvar)) {
                        this.constant = cvar;
                    } else {
                        console.trace();
                    }
                }
            },

            initializeFromHash: function(constant /*ClDouble*/, terms /*c.Hashtable*/) {
                if (c.verbose) {
                    console.log("*******************************");
                    console.log("clone c.initializeFromHash");
                    console.log("*******************************");
                }

                if (c.GC) console.log("clone c.Expression");
                this.constant = constant;
                this.terms = terms.clone();
                return this;
            },

            multiplyMe: function(x /*double*/) {
                this.constant *= x;
                var t = this.terms;
                t.each(function(clv, coeff) { t.set(clv, coeff * x); });
                return this;
            },

            clone: function() {
                if (c.verbose) {
                    console.log("*******************************");
                    console.log("clone c.Expression");
                    console.log("*******************************");
                }

                var e = c.Expression.empty();
                e.initializeFromHash(this.constant, this.terms);
                return e;
            },

            times: function(x) {
                if (typeof x == 'number') {
                    return (this.clone()).multiplyMe(x);
                } else {
                    if (this.isConstant) {
                        return x.times(this.constant);
                    } else if (x.isConstant) {
                        return this.times(x.constant);
                    } else {
                        throw new c.NonExpression();
                    }
                }
            },

            plus: function(expr /*c.Expression*/) {
                if (expr instanceof c.Expression) {
                    return this.clone().addExpression(expr, 1);
                } else if (expr instanceof c.Variable) {
                    return this.clone().addVariable(expr, 1);
                }
            },

            minus: function(expr /*c.Expression*/) {
                if (expr instanceof c.Expression) {
                    return this.clone().addExpression(expr, -1);
                } else if (expr instanceof c.Variable) {
                    return this.clone().addVariable(expr, -1);
                }
            },

            divide: function(x) {
                if (typeof x == 'number') {
                    if (c.approx(x, 0)) {
                        throw new c.NonExpression();
                    }
                    return this.times(1 / x);
                } else if (x instanceof c.Expression) {
                    if (!x.isConstant) {
                        throw new c.NonExpression();
                    }
                    return this.times(1 / x.constant);
                }
            },

            addExpression: function(expr /*c.Expression*/,
                                    n /*double*/,
                                    subject /*c.AbstractVariable*/,
                                    solver /*c.Tableau*/) {

                // console.log("c.Expression::addExpression()", expr, n);
                // console.trace();
                if (expr instanceof c.AbstractVariable) {
                    expr = c.Expression.fromVariable(expr);
                    // if(c.trace) console.log("addExpression: Had to cast a var to an expression");
                }
                n = checkNumber(n, 1);
                this.constant += (n * expr.constant);
                expr.terms.each(function(clv, coeff) {
                    // console.log("clv:", clv, "coeff:", coeff, "subject:", subject);
                    this.addVariable(clv, coeff * n, subject, solver);
                }, this);
                return this;
            },

            addVariable: function(v /*c.AbstractVariable*/, cd /*double*/, subject, solver) {
                if (cd == null) {
                    cd = 1;
                }

                /*
                  if (c.trace) console.log("c.Expression::addVariable():", v , cd);
                */
                var coeff = this.terms.get(v);
                if (coeff) {
                    var newCoefficient = coeff + cd;
                    if (newCoefficient == 0 || c.approx(newCoefficient, 0)) {
                        if (solver) {
                            solver.noteRemovedVariable(v, subject);
                        }
                        this.terms.delete(v);
                    } else {
                        this.setVariable(v, newCoefficient);
                    }
                } else {
                    if (!c.approx(cd, 0)) {
                        this.setVariable(v, cd);
                        if (solver) {
                            solver.noteAddedVariable(v, subject);
                        }
                    }
                }
                return this;
            },

            setVariable: function(v /*c.AbstractVariable*/, c /*double*/) {
                // console.log("terms.set(", v, c, ")");
                this.terms.set(v, c);
                return this;
            },

            anyPivotableVariable: function() {
                if (this.isConstant) {
                    throw new c.InternalError("anyPivotableVariable called on a constant");
                }

                var rv = this.terms.escapingEach(function(clv, c) {
                    if (clv.isPivotable) return { retval: clv };
                });

                if (rv && rv.retval !== undefined) {
                    return rv.retval;
                }

                return null;
            },

            substituteOut: function(outvar  /*c.AbstractVariable*/,
                                    expr    /*c.Expression*/,
                                    subject /*c.AbstractVariable*/,
                                    solver  /*ClTableau*/) {

                /*
                  if (c.trace) {
                  c.fnenterprint("CLE:substituteOut: " + outvar + ", " + expr + ", " + subject + ", ...");
                  c.traceprint("this = " + this);
                  }
                */
                var setVariable = this.setVariable.bind(this);
                var terms = this.terms;
                var multiplier = terms.get(outvar);
                terms.delete(outvar);
                this.constant += (multiplier * expr.constant);
                /*
                  console.log("substituteOut:",
                  "\n\toutvar:", outvar,
                  "\n\texpr:", expr.toString(),
                  "\n\tmultiplier:", multiplier,
                  "\n\tterms:", terms);
                */
                expr.terms.each(function(clv, coeff) {
                    var oldCoefficient = terms.get(clv);
                    if (oldCoefficient) {
                        var newCoefficient = oldCoefficient + multiplier * coeff;
                        if (c.approx(newCoefficient, 0)) {
                            solver.noteRemovedVariable(clv, subject);
                            terms.delete(clv);
                        } else {
                            terms.set(clv, newCoefficient);
                        }
                    } else {
                        terms.set(clv, multiplier * coeff);
                        if (solver) {
                            solver.noteAddedVariable(clv, subject);
                        }
                    }
                });
                // if (c.trace) c.traceprint("Now this is " + this);
            },

            changeSubject: function(old_subject /*c.AbstractVariable*/,
                                    new_subject /*c.AbstractVariable*/) {
                this.setVariable(old_subject, this.newSubject(new_subject));
            },

            newSubject: function(subject /*c.AbstractVariable*/) {
                // if (c.trace) c.fnenterprint("newSubject:" + subject);

                var reciprocal = 1 / this.terms.get(subject);
                this.terms.delete(subject);
                this.multiplyMe(-reciprocal);
                return reciprocal;
            },

            // Return the coefficient corresponding to variable var, i.e.,
            // the 'ci' corresponding to the 'vi' that var is:
            //     v1*c1 + v2*c2 + .. + vn*cn + c
            coefficientFor: function(clv /*c.AbstractVariable*/) {
                return this.terms.get(clv) || 0;
            },

            get isConstant() {
                return this.terms.size == 0;
            },

            toString: function() {
                var bstr = ''; // answer
                var needsplus = false;
                if (!c.approx(this.constant, 0) || this.isConstant) {
                    bstr += this.constant;
                    if (this.isConstant) {
                        return bstr;
                    } else {
                        needsplus = true;
                    }
                }
                this.terms.each( function(clv, coeff) {
                    if (needsplus) {
                        bstr += " + ";
                    }
                    bstr += coeff + "*" + clv;
                    needsplus = true;
                });
                return bstr;
            },

            equals: function(other) {
                if (other === this) {
                    return true;
                }

                return other instanceof c.Expression &&
                    other.constant === this.constant &&
                    other.terms.equals(this.terms);
            },

            Plus: function(e1 /*c.Expression*/, e2 /*c.Expression*/) {
                return e1.plus(e2);
            },

            Minus: function(e1 /*c.Expression*/, e2 /*c.Expression*/) {
                return e1.minus(e2);
            },

            Times: function(e1 /*c.Expression*/, e2 /*c.Expression*/) {
                return e1.times(e2);
            },

            Divide: function(e1 /*c.Expression*/, e2 /*c.Expression*/) {
                return e1.divide(e2);
            },
        });

        c.Expression.empty = function() {
            return new c.Expression(undefined, 1, 0);
        };

        c.Expression.fromConstant = function(cons) {
            return new c.Expression(cons);
        };

        c.Expression.fromValue = function(v) {
            v = +(v);
            return new c.Expression(undefined, v, 0);
        };

        c.Expression.fromVariable = function(v) {
            return new c.Expression(v, 1, 0);
        }

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011-2012, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";

        c.AbstractConstraint = c.inherit({
            initialize: function(strength /*c.Strength*/, weight /*double*/) {
                this.hashCode = c._inc();
                this.strength = strength || c.Strength.required;
                this.weight = weight || 1;
            },

            isEditConstraint: false,
            isInequality:     false,
            isStayConstraint: false,
            get required() { return (this.strength === c.Strength.required); },

            toString: function() {
                // this is abstract -- it intentionally leaves the parens unbalanced for
                // the subclasses to complete (e.g., with ' = 0', etc.
                return this.strength + " {" + this.weight + "} (" + this.expression +")";
            },
        });

        var ts = c.AbstractConstraint.prototype.toString;

        var EditOrStayCtor = function(cv /*c.Variable*/, strength /*c.Strength*/, weight /*double*/) {
            c.AbstractConstraint.call(this, strength || c.Strength.strong, weight);
            this.variable = cv;
            this.expression = new c.Expression(cv, -1, cv.value);
        };

        c.EditConstraint = c.inherit({
            extends: c.AbstractConstraint,
            initialize: function() { EditOrStayCtor.apply(this, arguments); },
            isEditConstraint: true,
            toString: function() { return "edit:" + ts.call(this); },
        });

        c.StayConstraint = c.inherit({
            extends: c.AbstractConstraint,
            initialize: function() { EditOrStayCtor.apply(this, arguments); },
            isStayConstraint: true,
            toString: function() { return "stay:" + ts.call(this); },
        });

        var lc =
            c.Constraint = c.inherit({
                extends: c.AbstractConstraint,
                initialize: function(cle /*c.Expression*/,
                                     strength /*c.Strength*/,
                                     weight /*double*/) {
                    c.AbstractConstraint.call(this, strength, weight);
                    this.expression = cle;
                },
            });

        c.Inequality = c.inherit({
            extends: c.Constraint,

            _cloneOrNewCle: function(cle) {
                // FIXME(D4): move somewhere else?
                if (cle.clone)  {
                    return cle.clone();
                } else {
                    return new c.Expression(cle);
                }
            },

            initialize: function(a1, a2, a3, a4, a5) {
                // FIXME(slightlyoff): what a disgusting mess. Should at least add docs.
                // console.log("c.Inequality.initialize(", a1, a2, a3, a4, a5, ")");

                var a1IsExp = a1 instanceof c.Expression,
                    a3IsExp = a3 instanceof c.Expression,
                    a1IsVar = a1 instanceof c.AbstractVariable,
                    a3IsVar = a3 instanceof c.AbstractVariable,
                    a1IsNum = typeof(a1) == 'number',
                    a3IsNum = typeof(a3) == 'number';

                // (cle || number), op, cv
                if ((a1IsExp || a1IsNum) && a3IsVar) {
                    var cle = a1, op = a2, cv = a3, strength = a4, weight = a5;
                    lc.call(this, this._cloneOrNewCle(cle), strength, weight);
                    if (op == c.LEQ) {
                        this.expression.multiplyMe(-1);
                        this.expression.addVariable(cv);
                    } else if (op == c.GEQ) {
                        this.expression.addVariable(cv, -1);
                    } else {
                        throw new c.InternalError("Invalid operator in c.Inequality constructor");
                    }
                    // cv, op, (cle || number)
                } else if (a1IsVar && (a3IsExp || a3IsNum)) {
                    var cle = a3, op = a2, cv = a1, strength = a4, weight = a5;
                    lc.call(this, this._cloneOrNewCle(cle), strength, weight);
                    if (op == c.GEQ) {
                        this.expression.multiplyMe(-1);
                        this.expression.addVariable(cv);
                    } else if (op == c.LEQ) {
                        this.expression.addVariable(cv, -1);
                    } else {
                        throw new c.InternalError("Invalid operator in c.Inequality constructor");
                    }
                    // cle, op, num
                } else if (a1IsExp && a3IsNum) {
                    var cle1 = a1, op = a2, cle2 = a3, strength = a4, weight = a5;
                    lc.call(this, this._cloneOrNewCle(cle1), strength, weight);
                    if (op == c.LEQ) {
                        this.expression.multiplyMe(-1);
                        this.expression.addExpression(this._cloneOrNewCle(cle2));
                    } else if (op == c.GEQ) {
                        this.expression.addExpression(this._cloneOrNewCle(cle2), -1);
                    } else {
                        throw new c.InternalError("Invalid operator in c.Inequality constructor");
                    }
                    return this
                    // num, op, cle
                } else if (a1IsNum && a3IsExp) {
                    var cle1 = a3, op = a2, cle2 = a1, strength = a4, weight = a5;
                    lc.call(this, this._cloneOrNewCle(cle1), strength, weight);
                    if (op == c.GEQ) {
                        this.expression.multiplyMe(-1);
                        this.expression.addExpression(this._cloneOrNewCle(cle2));
                    } else if (op == c.LEQ) {
                        this.expression.addExpression(this._cloneOrNewCle(cle2), -1);
                    } else {
                        throw new c.InternalError("Invalid operator in c.Inequality constructor");
                    }
                    return this
                    // cle op cle
                } else if (a1IsExp && a3IsExp) {
                    var cle1 = a1, op = a2, cle2 = a3, strength = a4, weight = a5;
                    lc.call(this, this._cloneOrNewCle(cle2), strength, weight);
                    if (op == c.GEQ) {
                        this.expression.multiplyMe(-1);
                        this.expression.addExpression(this._cloneOrNewCle(cle1));
                    } else if (op == c.LEQ) {
                        this.expression.addExpression(this._cloneOrNewCle(cle1), -1);
                    } else {
                        throw new c.InternalError("Invalid operator in c.Inequality constructor");
                    }
                    // cle
                } else if (a1IsExp) {
                    return lc.call(this, a1, a2, a3);
                    // >=
                } else if (a2 == c.GEQ) {
                    lc.call(this, new c.Expression(a3), a4, a5);
                    this.expression.multiplyMe(-1);
                    this.expression.addVariable(a1);
                    // <=
                } else if (a2 == c.LEQ) {
                    lc.call(this, new c.Expression(a3), a4, a5);
                    this.expression.addVariable(a1,-1);
                    // error
                } else {
                    throw new c.InternalError("Invalid operator in c.Inequality constructor");
                }
            },

            isInequality: true,

            toString: function() {
                // return "c.Inequality: " + this.hashCode;
                return lc.prototype.toString.call(this) + " >= 0) id: " + this.hashCode;
            },
        });

        c.Equation = c.inherit({
            extends: c.Constraint,
            initialize: function(a1, a2, a3, a4) {
                // FIXME(slightlyoff): this is just a huge mess.
                if (a1 instanceof c.Expression && !a2 || a2 instanceof c.Strength) {
                    lc.call(this, a1, a2, a3);
                } else if ((a1 instanceof c.AbstractVariable) &&
                           (a2 instanceof c.Expression)) {

                    var cv = a1, cle = a2, strength = a3, weight = a4;
                    lc.call(this, cle.clone(), strength, weight);
                    this.expression.addVariable(cv, -1);

                } else if ((a1 instanceof c.AbstractVariable) &&
                           (typeof(a2) == 'number')) {

                    var cv = a1, val = a2, strength = a3, weight = a4;
                    lc.call(this, new c.Expression(val), strength, weight);
                    this.expression.addVariable(cv, -1);

                } else if ((a1 instanceof c.Expression) &&
                           (a2 instanceof c.AbstractVariable)) {

                    var cle = a1, cv = a2, strength = a3, weight = a4;
                    lc.call(this, cle.clone(), strength, weight);
                    this.expression.addVariable(cv, -1);

                } else if (((a1 instanceof c.Expression) || (a1 instanceof c.AbstractVariable) ||
                            (typeof(a1) == 'number')) &&
                           ((a2 instanceof c.Expression) || (a2 instanceof c.AbstractVariable) ||
                            (typeof(a2) == 'number'))) {

                    if (a1 instanceof c.Expression) {
                        a1 = a1.clone();
                    } else {
                        a1 = new c.Expression(a1);
                    }

                    if (a2 instanceof c.Expression) {
                        a2 = a2.clone();
                    } else {
                        a2 = new c.Expression(a2);
                    }

                    lc.call(this, a1, a3, a4);
                    this.expression.addExpression(a2, -1);

                } else {
                    throw "Bad initializer to c.Equation";
                }
                c.assert(this.strength instanceof c.Strength, "_strength not set");
            },

            toString: function() {
                return lc.prototype.toString.call(this) + " = 0)";
            },
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";

        c.EditInfo = c.inherit({
            initialize: function(cn      /*c.Constraint*/,
                                 eplus   /*c.SlackVariable*/,
                                 eminus  /*c.SlackVariable*/,
                                 prevEditConstant /*double*/,
                                 i /*int*/) {
                this.constraint = cn;
                this.editPlus = eplus;
                this.editMinus = eminus;
                this.prevEditConstant = prevEditConstant;
                this.index = i;
            },
            toString: function() {
                return "<cn=" + this.constraint +
                    ", ep=" + this.editPlus +
                    ", em=" + this.editMinus +
                    ", pec=" + this.prevEditConstant +
                    ", index=" + this.index + ">";
            }
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";

        c.Tableau = c.inherit({
            initialize: function() {
                // columns is a mapping from variables which occur in expressions to the
                // set of basic variables whose expressions contain them
                // i.e., it's a mapping from variables in expressions (a column) to the
                // set of rows that contain them
                this.columns = new c.HashTable(); // values are sets

                // _rows maps basic variables to the expressions for that row in the tableau
                this.rows = new c.HashTable();    // values are c.Expressions

                // the collection of basic variables that have infeasible rows
                // (used when reoptimizing)
                this._infeasibleRows = new c.HashSet();

                // the set of rows where the basic variable is external this was added to
                // the C++ version to reduce time in setExternalVariables()
                this._externalRows = new c.HashSet();

                // the set of external variables which are parametric this was added to the
                // C++ version to reduce time in setExternalVariables()
                this._externalParametricVars = new c.HashSet();
            },

            // Variable v has been removed from an Expression.  If the Expression is in a
            // tableau the corresponding basic variable is subject (or if subject is nil
            // then it's in the objective function). Update the column cross-indices.
            noteRemovedVariable: function(v /*c.AbstractVariable*/,
                                          subject /*c.AbstractVariable*/) {
                c.trace && console.log("c.Tableau::noteRemovedVariable: ", v, subject);
                var column = this.columns.get(v);
                if (subject && column) {
                    column.delete(subject);
                }
            },

            noteAddedVariable: function(v /*c.AbstractVariable*/, subject /*c.AbstractVariable*/) {
                // if (c.trace) console.log("c.Tableau::noteAddedVariable:", v, subject);
                if (subject) {
                    this.insertColVar(v, subject);
                }
            },

            getInternalInfo: function() {
                var retstr = "Tableau Information:\n";
                retstr += "Rows: " + this.rows.size;
                retstr += " (= " + (this.rows.size - 1) + " constraints)";
                retstr += "\nColumns: " + this.columns.size;
                retstr += "\nInfeasible Rows: " + this._infeasibleRows.size;
                retstr += "\nExternal basic variables: " + this._externalRows.size;
                retstr += "\nExternal parametric variables: ";
                retstr += this._externalParametricVars.size;
                retstr += "\n";
                return retstr;
            },

            toString: function() {
                var bstr = "Tableau:\n";
                this.rows.each(function(clv, expr) {
                    bstr += clv;
                    bstr += " <==> ";
                    bstr += expr;
                    bstr += "\n";
                });
                bstr += "\nColumns:\n";
                bstr += this.columns;
                bstr += "\nInfeasible rows: ";
                bstr += this._infeasibleRows;
                bstr += "External basic variables: ";
                bstr += this._externalRows;
                bstr += "External parametric variables: ";
                bstr += this._externalParametricVars;
                return bstr;
            },

            /*
              toJSON: function() {
              // Creates an object representation of the Tableau.
              },
            */

            // Convenience function to insert a variable into
            // the set of rows stored at columns[param_var],
            // creating a new set if needed
            insertColVar: function(param_var /*c.Variable*/,
                                   rowvar /*c.Variable*/) {
                var rowset = /* Set */ this.columns.get(param_var);
                if (!rowset) {
                    rowset = new c.HashSet();
                    this.columns.set(param_var, rowset);
                }
                rowset.add(rowvar);
            },

            addRow: function(aVar /*c.AbstractVariable*/,
                             expr /*c.Expression*/) {
                if (c.trace) c.fnenterprint("addRow: " + aVar + ", " + expr);
                this.rows.set(aVar, expr);
                expr.terms.each(function(clv, coeff) {
                    this.insertColVar(clv, aVar);
                    if (clv.isExternal) {
                        this._externalParametricVars.add(clv);
                    }
                }, this);
                if (aVar.isExternal) {
                    this._externalRows.add(aVar);
                }
                if (c.trace) c.traceprint(this.toString());
            },

            removeColumn: function(aVar /*c.AbstractVariable*/) {
                if (c.trace) c.fnenterprint("removeColumn:" + aVar);
                var rows = /* Set */ this.columns.get(aVar);
                if (rows) {
                    this.columns.delete(aVar);
                    rows.each(function(clv) {
                        var expr = /* c.Expression */this.rows.get(clv);
                        expr.terms.delete(aVar);
                    }, this);
                } else {
                    if (c.trace) console.log("Could not find var", aVar, "in columns");
                }
                if (aVar.isExternal) {
                    this._externalRows.delete(aVar);
                    this._externalParametricVars.delete(aVar);
                }
            },

            removeRow: function(aVar /*c.AbstractVariable*/) {
                if (c.trace) c.fnenterprint("removeRow:" + aVar);
                var expr = /* c.Expression */this.rows.get(aVar);
                c.assert(expr != null);
                expr.terms.each(function(clv, coeff) {
                    var varset = this.columns.get(clv);
                    if (varset != null) {
                        if (c.trace) console.log("removing from varset:", aVar);
                        varset.delete(aVar);
                    }
                }, this);
                this._infeasibleRows.delete(aVar);
                if (aVar.isExternal) {
                    this._externalRows.delete(aVar);
                }
                this.rows.delete(aVar);
                if (c.trace) c.fnexitprint("returning " + expr);
                return expr;
            },

            substituteOut: function(oldVar /*c.AbstractVariable*/,
                                    expr /*c.Expression*/) {
                if (c.trace) c.fnenterprint("substituteOut:" + oldVar + ", " + expr);
                if (c.trace) c.traceprint(this.toString());

                var varset = this.columns.get(oldVar);
                varset.each(function(v) {
                    var row = this.rows.get(v);
                    row.substituteOut(oldVar, expr, v, this);
                    if (v.isRestricted && row.constant < 0) {
                        this._infeasibleRows.add(v);
                    }
                }, this);

                if (oldVar.isExternal) {
                    this._externalRows.add(oldVar);
                    this._externalParametricVars.delete(oldVar);
                }

                this.columns.delete(oldVar);
            },

            columnsHasKey: function(subject /*c.AbstractVariable*/) {
                return !!this.columns.get(subject);
            },
        });

    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        var t = c.Tableau;
        var tp = t.prototype;
        var epsilon = 1e-8;
        var weak = c.Strength.weak;

        c.SimplexSolver = c.inherit({
            extends: c.Tableau,
            initialize: function(){

                c.Tableau.call(this);
                this._stayMinusErrorVars = [];
                this._stayPlusErrorVars = [];

                this._errorVars = new c.HashTable(); // cn -> Set of cv

                this._markerVars = new c.HashTable(); // cn -> Set of cv

                // this._resolve_pair = [0, 0];
                this._objective = new c.ObjectiveVariable({ name: "Z" });

                this._editVarMap = new c.HashTable(); // cv -> c.EditInfo
                this._editVarList = [];

                this._slackCounter = 0;
                this._artificialCounter = 0;
                this._dummyCounter = 0;
                this.autoSolve = true;
                this._needsSolving = false;

                this._optimizeCount = 0;

                this.rows.set(this._objective, c.Expression.empty());
                this._editVariableStack = [0]; // Stack
                if (c.trace)
                    c.traceprint("objective expr == " + this.rows.get(this._objective));
            },

            add: function(/*c.Constraint, ...*/) {
                for (var x = 0; x < arguments.length; x++) {
                    this.addConstraint(arguments[x]);
                }
                return this;
            },

            _addEditConstraint: function(cn, eplus_eminus, prevEConstant) {
                var i = this._editVarMap.size;
                var cvEplus = /* c.SlackVariable */eplus_eminus[0];
                var cvEminus = /* c.SlackVariable */eplus_eminus[1];
                /*
                  if (!cvEplus instanceof c.SlackVariable) {
                  console.warn("cvEplus not a slack variable =", cvEplus);
                  }
                  if (!cvEminus instanceof c.SlackVariable) {
                  console.warn("cvEminus not a slack variable =", cvEminus);
                  }
                  c.debug && console.log("new c.EditInfo(" + cn + ", " + cvEplus + ", " +
                  cvEminus + ", " + prevEConstant + ", " +
                  i +")");
                */
                var ei = new c.EditInfo(cn, cvEplus, cvEminus, prevEConstant, i)
                this._editVarMap.set(cn.variable, ei);
                this._editVarList[i] = { v: cn.variable, info: ei };
            },

            addConstraint: function(cn /*c.Constraint*/) {
                c.trace && c.fnenterprint("addConstraint: " + cn);
                var eplus_eminus = new Array(2);
                var prevEConstant = new Array(1); // so it can be output to
                var expr = this.newExpression(cn, /*output to*/ eplus_eminus, prevEConstant);
                prevEConstant = prevEConstant[0];

                if (!this.tryAddingDirectly(expr)) {
                    this.addWithArtificialVariable(expr);
                }


                this._needsSolving = true;
                if (cn.isEditConstraint) {
                    this._addEditConstraint(cn, eplus_eminus, prevEConstant);
                }
                if (this.autoSolve) {
                    this.optimize(this._objective);
                    this._setExternalVariables();
                }
                return this;
            },

            addConstraintNoException: function(cn /*c.Constraint*/) {
                c.trace && c.fnenterprint("addConstraintNoException: " + cn);
                // FIXME(slightlyoff): change this to enable chaining
                try {
                    this.addConstraint(cn);
                    return true;
                } catch (e /*c.RequiredFailure*/){
                    return false;
                }
            },

            addEditVar: function(v /*c.Variable*/, strength /*c.Strength*/, weight /*double*/) {
                c.trace && c.fnenterprint("addEditVar: " + v + " @ " + strength + " {" + weight + "}");
                return this.addConstraint(
                    new c.EditConstraint(v, strength || c.Strength.strong, weight));
            },

            beginEdit: function() {
                // FIXME(slightlyoff): we shouldn't throw here. Log instead
                c.assert(this._editVarMap.size > 0, "_editVarMap.size > 0");
                this._infeasibleRows.clear();
                this._resetStayConstants();
                this._editVariableStack[this._editVariableStack.length] = this._editVarMap.size;
                return this;
            },

            endEdit: function() {
                // FIXME(slightlyoff): we shouldn't throw here. Log instead
                c.assert(this._editVarMap.size > 0, "_editVarMap.size > 0");
                this.resolve();
                this._editVariableStack.pop();
                this.removeEditVarsTo(
                    this._editVariableStack[this._editVariableStack.length - 1]
                );
                return this;
            },

            removeAllEditVars: function() {
                return this.removeEditVarsTo(0);
            },

            removeEditVarsTo: function(n /*int*/) {
                try {
                    var evll = this._editVarList.length;
                    // only remove the variable if it's not in the set of variable
                    // from a previous nested outer edit
                    // e.g., if I do:
                    // Edit x,y
                    // Edit w,h,x,y
                    // EndEdit
                    // The end edit needs to only get rid of the edits on w,h
                    // not the ones on x,y
                    for(var x = n; x < evll; x++) {
                        if (this._editVarList[x]) {
                            this.removeConstraint(
                                this._editVarMap.get(this._editVarList[x].v).constraint
                            );
                        }
                    }
                    this._editVarList.length = n;
                    c.assert(this._editVarMap.size == n, "_editVarMap.size == n");
                    return this;
                } catch (e /*ConstraintNotFound*/){
                    throw new c.InternalError("Constraint not found in removeEditVarsTo");
                }
            },

            // Add weak stays to the x and y parts of each point. These have
            // increasing weights so that the solver will try to satisfy the x
            // and y stays on the same point, rather than the x stay on one and
            // the y stay on another.
            addPointStays: function(points /*[{ x: .., y: ..}, ...]*/) {
                c.trace && console.log("addPointStays", points);
                points.forEach(function(p, idx) {
                    this.addStay(p.x, weak, Math.pow(2, idx));
                    this.addStay(p.y, weak, Math.pow(2, idx));
                }, this);
                return this;
            },

            addStay: function(v /*c.Variable*/, strength /*c.Strength*/, weight /*double*/) {
                var cn = new c.StayConstraint(v,
                                              strength || weak,
                                              weight   || 1);
                return this.addConstraint(cn);
            },

            // FIXME(slightlyoff): add a removeStay

            removeConstraint: function(cn /*c.Constraint*/) {
                // console.log("removeConstraint('", cn, "')");
                c.trace && c.fnenterprint("removeConstraintInternal: " + cn);
                c.trace && c.traceprint(this.toString());
                this._needsSolving = true;
                this._resetStayConstants();
                var zRow = this.rows.get(this._objective);
                var eVars = /* Set */this._errorVars.get(cn);
                c.trace && c.traceprint("eVars == " + eVars);
                if (eVars != null) {
                    eVars.each(function(cv) {
                        var expr = this.rows.get(cv);
                        if (expr == null) {
                            zRow.addVariable(cv,
                                             -cn.weight * cn.strength.symbolicWeight.value,
                                             this._objective,
                                             this);
                        } else {
                            zRow.addExpression(expr,
                                               -cn.weight * cn.strength.symbolicWeight.value,
                                               this._objective,
                                               this);
                        }
                        c.trace && c.traceprint("now eVars == " + eVars);
                    }, this);
                }
                var marker = this._markerVars.get(cn);
                this._markerVars.delete(cn);
                if (marker == null) {
                    throw new c.InternalError("Constraint not found in removeConstraintInternal");
                }
                c.trace && c.traceprint("Looking to remove var " + marker);
                if (this.rows.get(marker) == null) {
                    var col = this.columns.get(marker);
                    // console.log("col is:", col, "from marker:", marker);
                    c.trace && c.traceprint("Must pivot -- columns are " + col);
                    var exitVar = null;
                    var minRatio = 0;
                    col.each(function(v) {
                        if (v.isRestricted) {
                            var expr = this.rows.get(v);
                            var coeff = expr.coefficientFor(marker);
                            c.trace && c.traceprint("Marker " + marker + "'s coefficient in " + expr + " is " + coeff);
                            if (coeff < 0) {
                                var r = -expr.constant / coeff;
                                if (
                                    exitVar == null ||
                                        r < minRatio    ||
                                        (c.approx(r, minRatio) && v.hashCode < exitVar.hashCode)
                                ) {
                                    minRatio = r;
                                    exitVar = v;
                                }
                            }
                        }
                    }, this);
                    if (exitVar == null) {
                        c.trace && c.traceprint("exitVar is still null");
                        col.each(function(v) {
                            if (v.isRestricted) {
                                var expr = this.rows.get(v);
                                var coeff = expr.coefficientFor(marker);
                                var r = expr.constant / coeff;
                                if (exitVar == null || r < minRatio) {
                                    minRatio = r;
                                    exitVar = v;
                                }
                            }
                        }, this);
                    }
                    if (exitVar == null) {
                        if (col.size == 0) {
                            this.removeColumn(marker);
                        } else {
                            col.escapingEach(function(v) {
                                if (v != this._objective) {
                                    exitVar = v;
                                    return { brk: true };
                                }
                            }, this);
                        }
                    }
                    if (exitVar != null) {
                        this.pivot(marker, exitVar);
                    }
                }
                if (this.rows.get(marker) != null) {
                    var expr = this.removeRow(marker);
                }

                if (eVars != null) {
                    eVars.each(function(v) {
                        if (v != marker) { this.removeColumn(v); }
                    }, this);
                }

                if (cn.isStayConstraint) {
                    if (eVars != null) {
                        for (var i = 0; i < this._stayPlusErrorVars.length; i++) {
                            eVars.delete(this._stayPlusErrorVars[i]);
                            eVars.delete(this._stayMinusErrorVars[i]);
                        }
                    }
                } else if (cn.isEditConstraint) {
                    c.assert(eVars != null, "eVars != null");
                    var cei = this._editVarMap.get(cn.variable);
                    this.removeColumn(cei.editMinus);
                    this._editVarMap.delete(cn.variable);
                }

                if (eVars != null) {
                    this._errorVars.delete(eVars);
                }

                if (this.autoSolve) {
                    this.optimize(this._objective);
                    this._setExternalVariables();
                }

                return this;
            },

            reset: function() {
                c.trace && c.fnenterprint("reset");
                throw new c.InternalError("reset not implemented");
            },

            resolveArray: function(newEditConstants) {
                c.trace && c.fnenterprint("resolveArray" + newEditConstants);
                var l = newEditConstants.length
                this._editVarMap.each(function(v, cei) {
                    var i = cei.index;
                    if (i < l)
                        this.suggestValue(v, newEditConstants[i]);
                }, this);
                this.resolve();
            },

            resolvePair: function(x /*double*/, y /*double*/) {
                this.suggestValue(this._editVarList[0].v, x);
                this.suggestValue(this._editVarList[1].v, y);
                this.resolve();
            },

            resolve: function() {
                c.trace && c.fnenterprint("resolve()");
                this.dualOptimize();
                this._setExternalVariables();
                this._infeasibleRows.clear();
                this._resetStayConstants();
            },

            suggestValue: function(v /*c.Variable*/, x /*double*/) {
                c.trace && console.log("suggestValue(" + v + ", " + x + ")");
                var cei = this._editVarMap.get(v);
                if (!cei) {
                    throw new c.Error("suggestValue for variable " + v + ", but var is not an edit variable");
                }
                var delta = x - cei.prevEditConstant;
                cei.prevEditConstant = x;
                this.deltaEditConstant(delta, cei.editPlus, cei.editMinus);
                return this;
            },

            solve: function() {
                if (this._needsSolving) {
                    this.optimize(this._objective);
                    this._setExternalVariables();
                }
                return this;
            },

            setEditedValue: function(v /*c.Variable*/, n /*double*/) {
                if (!(this.columnsHasKey(v) || (this.rows.get(v) != null))) {
                    v.value = n;
                    return this;
                }

                if (!c.approx(n, v.value)) {
                    this.addEditVar(v);
                    this.beginEdit();

                    try {
                        this.suggestValue(v, n);
                    } catch (e) {
                        throw new c.InternalError("Error in setEditedValue");
                    }

                    this.endEdit();
                }
                return this;
            },

            addVar: function(v /*c.Variable*/) {
                if (!(this.columnsHasKey(v) || (this.rows.get(v) != null))) {
                    try {
                        this.addStay(v);
                    } catch (e /*c.RequiredFailure*/){
                        throw new c.InternalError("Error in addVar -- required failure is impossible");
                    }

                    c.trace && c.traceprint("added initial stay on " + v);
                }
                return this;
            },

            getInternalInfo: function() {
                var retstr = tp.getInternalInfo.call(this);
                retstr += "\nSolver info:\n";
                retstr += "Stay Error Variables: ";
                retstr += this._stayPlusErrorVars.length + this._stayMinusErrorVars.length;
                retstr += " (" + this._stayPlusErrorVars.length + " +, ";
                retstr += this._stayMinusErrorVars.length + " -)\n";
                retstr += "Edit Variables: " + this._editVarMap.size;
                retstr += "\n";
                return retstr;
            },

            getDebugInfo: function() {
                return this.toString() + this.getInternalInfo() + "\n";
            },

            toString: function() {
                var bstr = tp.getInternalInfo.call(this);
                bstr += "\n_stayPlusErrorVars: ";
                bstr += '[' + this._stayPlusErrorVars + ']';
                bstr += "\n_stayMinusErrorVars: ";
                bstr += '[' + this._stayMinusErrorVars + ']';
                bstr += "\n";
                bstr += "_editVarMap:\n" + this._editVarMap;
                bstr += "\n";
                return bstr;
            },

            addWithArtificialVariable: function(expr /*c.Expression*/) {
                c.trace && c.fnenterprint("addWithArtificialVariable: " + expr);
                var av = new c.SlackVariable({
                    value: ++this._artificialCounter,
                    prefix: "a"
                });
                var az = new c.ObjectiveVariable({ name: "az" });
                var azRow = /* c.Expression */expr.clone();
                c.trace && c.traceprint("before addRows:\n" + this);
                this.addRow(az, azRow);
                this.addRow(av, expr);
                c.trace && c.traceprint("after addRows:\n" + this);
                this.optimize(az);
                var azTableauRow = this.rows.get(az);
                c.trace && c.traceprint("azTableauRow.constant == " + azTableauRow.constant);
                if (!c.approx(azTableauRow.constant, 0)) {
                    this.removeRow(az);
                    this.removeColumn(av);
                    throw new c.RequiredFailure();
                }
                var e = this.rows.get(av);
                if (e != null) {
                    if (e.isConstant) {
                        this.removeRow(av);
                        this.removeRow(az);
                        return;
                    }
                    var entryVar = e.anyPivotableVariable();
                    this.pivot(entryVar, av);
                }
                c.assert(this.rows.get(av) == null, "rowExpression(av) == null");
                this.removeColumn(av);
                this.removeRow(az);
            },

            tryAddingDirectly: function(expr /*c.Expression*/) {
                c.trace && c.fnenterprint("tryAddingDirectly: " + expr);
                var subject = this.chooseSubject(expr);
                if (subject == null) {
                    c.trace && c.fnexitprint("returning false");
                    return false;
                }
                expr.newSubject(subject);
                if (this.columnsHasKey(subject)) {
                    this.substituteOut(subject, expr);
                }
                this.addRow(subject, expr);
                c.trace && c.fnexitprint("returning true");
                return true;
            },

            chooseSubject: function(expr /*c.Expression*/) {
                c.trace && c.fnenterprint("chooseSubject: " + expr);
                var subject = null;
                var foundUnrestricted = false;
                var foundNewRestricted = false;
                var terms = expr.terms;
                var rv = terms.escapingEach(function(v, c) {
                    if (foundUnrestricted) {
                        if (!v.isRestricted) {
                            if (!this.columnsHasKey(v)) {
                                return { retval: v };
                            }
                        }
                    } else {
                        if (v.isRestricted) {
                            if (!foundNewRestricted && !v.isDummy && c < 0) {
                                var col = this.columns.get(v);
                                if (col == null ||
                                    (col.size == 1 && this.columnsHasKey(this._objective))
                                   ) {
                                    subject = v;
                                    foundNewRestricted = true;
                                }
                            }
                        } else {
                            subject = v;
                            foundUnrestricted = true;
                        }
                    }
                }, this);
                if (rv && rv.retval !== undefined) {
                    return rv.retval;
                }

                if (subject != null) {
                    return subject;
                }

                var coeff = 0;

                // subject is nil.
                // Make one last check -- if all of the variables in expr are dummy
                // variables, then we can pick a dummy variable as the subject
                var rv = terms.escapingEach(function(v,c) {
                    if (!v.isDummy)  {
                        return {retval:null};
                    }
                    if (!this.columnsHasKey(v)) {
                        subject = v;
                        coeff = c;
                    }
                }, this);
                if (rv && rv.retval !== undefined) return rv.retval;

                if (!c.approx(expr.constant, 0)) {
                    throw new c.RequiredFailure();
                }
                if (coeff > 0) {
                    expr.multiplyMe(-1);
                }
                return subject;
            },

            deltaEditConstant: function(delta /*double*/,
                                        plusErrorVar /*c.AbstractVariable*/,
                                        minusErrorVar /*c.AbstractVariable*/) {
                if (c.trace)
                    c.fnenterprint("deltaEditConstant :" + delta + ", " + plusErrorVar + ", " + minusErrorVar);

                var exprPlus = this.rows.get(plusErrorVar);
                if (exprPlus != null) {
                    exprPlus.constant += delta;
                    if (exprPlus.constant < 0) {
                        this._infeasibleRows.add(plusErrorVar);
                    }
                    return;
                }
                var exprMinus = this.rows.get(minusErrorVar);
                if (exprMinus != null) {
                    exprMinus.constant += -delta;
                    if (exprMinus.constant < 0) {
                        this._infeasibleRows.add(minusErrorVar);
                    }
                    return;
                }
                var columnVars = this.columns.get(minusErrorVar);
                if (!columnVars) {
                    console.log("columnVars is null -- tableau is:\n" + this);
                }
                columnVars.each(function(basicVar) {
                    var expr = this.rows.get(basicVar);
                    var c = expr.coefficientFor(minusErrorVar);
                    expr.constant += (c * delta);
                    if (basicVar.isRestricted && expr.constant < 0) {
                        this._infeasibleRows.add(basicVar);
                    }
                }, this);
            },

            // We have set new values for the constants in the edit constraints.
            // Re-Optimize using the dual simplex algorithm.
            dualOptimize: function() {
                c.trace && c.fnenterprint("dualOptimize:");
                var zRow = this.rows.get(this._objective);
                // need to handle infeasible rows
                while (this._infeasibleRows.size) {
                    var exitVar = this._infeasibleRows.values()[0];
                    this._infeasibleRows.delete(exitVar);
                    var entryVar = null;
                    var expr = this.rows.get(exitVar);
                    // exitVar might have become basic after some other pivoting
                    // so allow for the case of its not being there any longer
                    if (expr) {
                        if (expr.constant < 0) {
                            var ratio = Number.MAX_VALUE;
                            var r;
                            var terms = expr.terms;
                            terms.each(function(v, cd) {
                                if (cd > 0 && v.isPivotable) {
                                    var zc = zRow.coefficientFor(v);
                                    r = zc / cd;
                                    if (r < ratio ||
                                        (c.approx(r, ratio) && v.hashCode < entryVar.hashCode)
                                       ) {
                                        entryVar = v;
                                        ratio = r;
                                    }
                                }
                            });
                            if (ratio == Number.MAX_VALUE) {
                                throw new c.InternalError("ratio == nil (MAX_VALUE) in dualOptimize");
                            }
                            this.pivot(entryVar, exitVar);
                        }
                    }
                }
            },

            // Make a new linear Expression representing the constraint cn,
            // replacing any basic variables with their defining expressions.
            // Normalize if necessary so that the Constant is non-negative.  If
            // the constraint is non-required give its error variables an
            // appropriate weight in the objective function.
            newExpression: function(cn /*c.Constraint*/,
                                    /** outputs to **/ eplus_eminus /*Array*/,
                                    prevEConstant) {
                if (c.trace) {
                    c.fnenterprint("newExpression: " + cn);
                    c.traceprint("cn.isInequality == " + cn.isInequality);
                    c.traceprint("cn.required == " + cn.required);
                }

                var cnExpr = cn.expression;
                var expr = c.Expression.fromConstant(cnExpr.constant);
                var slackVar = new c.SlackVariable();
                var dummyVar = new c.DummyVariable();
                var eminus = new c.SlackVariable();
                var eplus = new c.SlackVariable();
                var cnTerms = cnExpr.terms;
                // console.log(cnTerms.size);

                cnTerms.each(function(v, c) {
                    var e = this.rows.get(v);
                    if (!e) {
                        expr.addVariable(v, c);
                    } else {
                        expr.addExpression(e, c);
                    }
                }, this);

                if (cn.isInequality) {
                    // cn is an inequality, so Add a slack variable. The original constraint
                    // is expr>=0, so that the resulting equality is expr-slackVar=0. If cn is
                    // also non-required Add a negative error variable, giving:
                    //
                    //    expr - slackVar = -errorVar
                    //
                    // in other words:
                    //
                    //    expr - slackVar + errorVar = 0
                    //
                    // Since both of these variables are newly created we can just Add
                    // them to the Expression (they can't be basic).
                    c.trace && c.traceprint("Inequality, adding slack");
                    ++this._slackCounter;
                    slackVar = new c.SlackVariable({
                        value: this._slackCounter,
                        prefix: "s"
                    });
                    expr.setVariable(slackVar, -1);

                    this._markerVars.set(cn, slackVar);
                    if (!cn.required) {
                        ++this._slackCounter;
                        eminus = new c.SlackVariable({
                            value: this._slackCounter,
                            prefix: "em"
                        });
                        expr.setVariable(eminus, 1);
                        var zRow = this.rows.get(this._objective);
                        zRow.setVariable(eminus, cn.strength.symbolicWeight.value * cn.weight);
                        this.insertErrorVar(cn, eminus);
                        this.noteAddedVariable(eminus, this._objective);
                    }
                } else {
                    if (cn.required) {
                        c.trace && c.traceprint("Equality, required");
                        // Add a dummy variable to the Expression to serve as a marker for this
                        // constraint.  The dummy variable is never allowed to enter the basis
                        // when pivoting.
                        ++this._dummyCounter;
                        dummyVar = new c.DummyVariable({
                            value: this._dummyCounter,
                            prefix: "d"
                        });
                        eplus_eminus[0] = dummyVar;
                        eplus_eminus[1] = dummyVar;
                        prevEConstant[0] = cnExpr.constant;
                        expr.setVariable(dummyVar, 1);
                        this._markerVars.set(cn, dummyVar);
                        c.trace && c.traceprint("Adding dummyVar == d" + this._dummyCounter);
                    } else {
                        // cn is a non-required equality. Add a positive and a negative error
                        // variable, making the resulting constraint
                        //       expr = eplus - eminus
                        // in other words:
                        //       expr - eplus + eminus = 0
                        c.trace && c.traceprint("Equality, not required");
                        ++this._slackCounter;
                        eplus = new c.SlackVariable({
                            value: this._slackCounter,
                            prefix: "ep"
                        });
                        eminus = new c.SlackVariable({
                            value: this._slackCounter,
                            prefix: "em"
                        });
                        expr.setVariable(eplus, -1);
                        expr.setVariable(eminus, 1);
                        this._markerVars.set(cn, eplus);
                        var zRow = this.rows.get(this._objective);
                        c.trace && console.log(zRow);
                        var swCoeff = cn.strength.symbolicWeight.value * cn.weight;
                        if (swCoeff == 0) {
                            c.trace && c.traceprint("cn == " + cn);
                            c.trace && c.traceprint("adding " + eplus + " and " + eminus + " with swCoeff == " + swCoeff);
                        }
                        zRow.setVariable(eplus, swCoeff);
                        this.noteAddedVariable(eplus, this._objective);
                        zRow.setVariable(eminus, swCoeff);
                        this.noteAddedVariable(eminus, this._objective);

                        this.insertErrorVar(cn, eminus);
                        this.insertErrorVar(cn, eplus);

                        if (cn.isStayConstraint) {
                            this._stayPlusErrorVars[this._stayPlusErrorVars.length] = eplus;
                            this._stayMinusErrorVars[this._stayMinusErrorVars.length] = eminus;
                        } else if (cn.isEditConstraint) {
                            eplus_eminus[0] = eplus;
                            eplus_eminus[1] = eminus;
                            prevEConstant[0] = cnExpr.constant;
                        }
                    }
                }
                // the Constant in the Expression should be non-negative. If necessary
                // normalize the Expression by multiplying by -1
                if (expr.constant < 0) expr.multiplyMe(-1);
                c.trace && c.fnexitprint("returning " + expr);
                return expr;
            },

            // Minimize the value of the objective.  (The tableau should already be
            // feasible.)
            optimize: function(zVar /*c.ObjectiveVariable*/) {
                c.trace && c.fnenterprint("optimize: " + zVar);
                c.trace && c.traceprint(this.toString());
                this._optimizeCount++;

                var zRow = this.rows.get(zVar);
                c.assert(zRow != null, "zRow != null");
                var entryVar = null;
                var exitVar = null;
                var objectiveCoeff, terms;

                while (true) {
                    objectiveCoeff = 0;
                    terms = zRow.terms;

                    // Find the most negative coefficient in the objective function (ignoring
                    // the non-pivotable dummy variables). If all coefficients are positive
                    // we're done
                    terms.escapingEach(function(v, c) {
                        if (v.isPivotable && c < objectiveCoeff) {
                            objectiveCoeff = c;
                            entryVar = v;
                            // Break on success
                            return { brk: 1 };
                        }
                    }, this);

                    if (objectiveCoeff >= -epsilon)
                        return;

                    c.trace && console.log("entryVar:", entryVar,
                                           "objectiveCoeff:", objectiveCoeff);

                    // choose which variable to move out of the basis
                    // Only consider pivotable basic variables
                    // (i.e. restricted, non-dummy variables)
                    var minRatio = Number.MAX_VALUE;
                    var columnVars = this.columns.get(entryVar);
                    var r = 0;

                    columnVars.each(function(v) {
                        c.trace && c.traceprint("Checking " + v);
                        if (v.isPivotable) {
                            var expr = this.rows.get(v);
                            var coeff = expr.coefficientFor(entryVar);
                            c.trace && c.traceprint("pivotable, coeff = " + coeff);
                            // only consider negative coefficients
                            if (coeff < 0) {
                                r = -expr.constant / coeff;
                                // Bland's anti-cycling rule:
                                // if multiple variables are about the same,
                                // always pick the lowest via some total
                                // ordering -- I use their addresses in memory
                                //    if (r < minRatio ||
                                //              (c.approx(r, minRatio) &&
                                //               v.get_pclv() < exitVar.get_pclv()))
                                if (r < minRatio ||
                                    (c.approx(r, minRatio) &&
                                     v.hashCode < exitVar.hashCode)
                                   ) {
                                    minRatio = r;
                                    exitVar = v;
                                }
                            }
                        }
                    }, this);

                    // If minRatio is still nil at this point, it means that the
                    // objective function is unbounded, i.e. it can become
                    // arbitrarily negative.  This should never happen in this
                    // application.
                    if (minRatio == Number.MAX_VALUE) {
                        throw new c.InternalError("Objective function is unbounded in optimize");
                    }

                    // console.time("SimplexSolver::optimize pivot()");
                    this.pivot(entryVar, exitVar);
                    // console.timeEnd("SimplexSolver::optimize pivot()");

                    c.trace && c.traceprint(this.toString());
                }
            },

            // Do a Pivot.  Move entryVar into the basis (i.e. make it a basic variable),
            // and move exitVar out of the basis (i.e., make it a parametric variable)
            pivot: function(entryVar /*c.AbstractVariable*/, exitVar /*c.AbstractVariable*/) {
                c.trace && console.log("pivot: ", entryVar, exitVar);
                var time = false;

                time && console.time(" SimplexSolver::pivot");

                // the entryVar might be non-pivotable if we're doing a RemoveConstraint --
                // otherwise it should be a pivotable variable -- enforced at call sites,
                // hopefully
                if (entryVar == null) {
                    console.warn("pivot: entryVar == null");
                }

                if (exitVar == null) {
                    console.warn("pivot: exitVar == null");
                }
                // console.log("SimplexSolver::pivot(", entryVar, exitVar, ")")

                // expr is the Expression for the exit variable (about to leave the basis) --
                // so that the old tableau includes the equation:
                //   exitVar = expr
                time && console.time("  removeRow");
                var expr = this.removeRow(exitVar);
                time && console.timeEnd("  removeRow");

                // Compute an Expression for the entry variable.  Since expr has
                // been deleted from the tableau we can destructively modify it to
                // build this Expression.
                time && console.time("  changeSubject");
                expr.changeSubject(exitVar, entryVar);
                time && console.timeEnd("  changeSubject");

                time && console.time("  substituteOut");
                this.substituteOut(entryVar, expr);
                time && console.timeEnd("  substituteOut");
                /*
                  if (entryVar.isExternal) {
                  // entry var is no longer a parametric variable since we're moving
                  // it into the basis
                  console.log("entryVar is external!");
                  this._externalParametricVars.delete(entryVar);
                  }
                */

                time && console.time("  addRow")
                this.addRow(entryVar, expr);
                time && console.timeEnd("  addRow")

                time && console.timeEnd(" SimplexSolver::pivot");
            },

            // Each of the non-required stays will be represented by an equation
            // of the form
            //     v = c + eplus - eminus
            // where v is the variable with the stay, c is the previous value of
            // v, and eplus and eminus are slack variables that hold the error
            // in satisfying the stay constraint.  We are about to change
            // something, and we want to fix the constants in the equations
            // representing the stays.  If both eplus and eminus are nonbasic
            // they have value 0 in the current solution, meaning the previous
            // stay was exactly satisfied.  In this case nothing needs to be
            // changed.  Otherwise one of them is basic, and the other must
            // occur only in the Expression for that basic error variable.
            // Reset the Constant in this Expression to 0.
            _resetStayConstants: function() {
                c.trace && console.log("_resetStayConstants");
                var spev = this._stayPlusErrorVars;
                var l = spev.length;
                for (var i = 0; i < l; i++) {
                    var expr = this.rows.get(spev[i]);
                    if (expr === null) {
                        expr = this.rows.get(this._stayMinusErrorVars[i]);
                    }
                    if (expr != null) {
                        expr.constant = 0;
                    }
                }
            },

            _setExternalVariables: function() {
                c.trace && c.fnenterprint("_setExternalVariables:");
                c.trace && c.traceprint(this.toString());
                var changed = {};

                // console.log("this._externalParametricVars:", this._externalParametricVars);
                this._externalParametricVars.each(function(v) {
                    if (this.rows.get(v) != null) {
                        if (c.trace)
                            console.log("Error: variable" + v + " in _externalParametricVars is basic");
                    } else {
                        v.value = 0;
                        changed[v.name] = 0;
                    }
                }, this);
                // console.log("this._externalRows:", this._externalRows);
                this._externalRows.each(function(v) {
                    var expr = this.rows.get(v);
                    if (v.value != expr.constant) {
                        // console.log(v.toString(), v.value, expr.constant);
                        v.value = expr.constant;
                        changed[v.name] = expr.constant;
                    }
                    // c.trace && console.log("v == " + v);
                    // c.trace && console.log("expr == " + expr);
                }, this);
                this._changed = changed;
                this._needsSolving = false;
                this._informCallbacks();
                this.onsolved();
            },

            onsolved: function() {
                // Lifecycle stub. Here for dirty, dirty monkey patching.
            },

            _informCallbacks: function() {
                if(!this._callbacks) return;

                var changed = this._changed;
                this._callbacks.forEach(function(fn) {
                    fn(changed);
                });
            },

            _addCallback: function(fn) {
                var a = (this._callbacks || (this._callbacks = []));
                a[a.length] = fn;
            },

            insertErrorVar: function(cn /*c.Constraint*/, aVar /*c.AbstractVariable*/) {
                c.trace && c.fnenterprint("insertErrorVar:" + cn + ", " + aVar);
                var constraintSet = /* Set */this._errorVars.get(aVar);
                if (!constraintSet) {
                    constraintSet = new c.HashSet();
                    this._errorVars.set(cn, constraintSet);
                }
                constraintSet.add(aVar);
            },
        });
    })(this["c"]||module.parent.exports||{});
    // Copyright (C) 1998-2000 Greg J. Badros
    // Use of this source code is governed by http://www.apache.org/licenses/LICENSE-2.0
    //
    // Parts Copyright (C) 2011, Alex Russell (slightlyoff@chromium.org)

    (function(c) {
        "use strict";

        c.Timer = c.inherit({
            initialize: function() {
                this.isRunning = false;
                this._elapsedMs = 0;
            },

            start: function() {
                this.isRunning = true;
                this._startReading = new Date();
                return this;
            },

            stop: function() {
                this.isRunning = false;
                this._elapsedMs += (new Date()) - this._startReading;
                return this;
            },

            reset: function() {
                this.isRunning = false;
                this._elapsedMs = 0;
                return this;
            },

            elapsedTime : function() {
                if (!this.isRunning) {
                    return this._elapsedMs / 1000;
                } else {
                    return (this._elapsedMs + (new Date() - this._startReading)) / 1000;
                }
            },
        });

    })(this["c"]||module.parent.exports||{});
    this.c.parser = (function() {
        /*
         * Generated by PEG.js 0.8.0.
         *
         * http://pegjs.majda.cz/
         */

        function peg$subclass(child, parent) {
            function ctor() { this.constructor = child; }
            ctor.prototype = parent.prototype;
            child.prototype = new ctor();
        }

        function SyntaxError(message, expected, found, offset, line, column) {
            this.message  = message;
            this.expected = expected;
            this.found    = found;
            this.offset   = offset;
            this.line     = line;
            this.column   = column;

            this.name     = "SyntaxError";
        }

        peg$subclass(SyntaxError, Error);

        function parse(input) {
            var options = arguments.length > 1 ? arguments[1] : {},

                peg$FAILED = {},

                peg$startRuleFunctions = { start: peg$parsestart },
                peg$startRuleFunction  = peg$parsestart,

                peg$c0 = peg$FAILED,
                peg$c1 = [],
                peg$c2 = function(statements) { return statements; },
                peg$c3 = function(expression) { return expression; },
                peg$c4 = { type: "any", description: "any character" },
                peg$c5 = /^[a-zA-Z]/,
                peg$c6 = { type: "class", value: "[a-zA-Z]", description: "[a-zA-Z]" },
                peg$c7 = "$",
                peg$c8 = { type: "literal", value: "$", description: "\"$\"" },
                peg$c9 = "_",
                peg$c10 = { type: "literal", value: "_", description: "\"_\"" },
                peg$c11 = { type: "other", description: "whitespace" },
                peg$c12 = /^[\t\x0B\f \xA0\uFEFF]/,
                peg$c13 = { type: "class", value: "[\\t\\x0B\\f \\xA0\\uFEFF]", description: "[\\t\\x0B\\f \\xA0\\uFEFF]" },
                peg$c14 = /^[\n\r\u2028\u2029]/,
                peg$c15 = { type: "class", value: "[\\n\\r\\u2028\\u2029]", description: "[\\n\\r\\u2028\\u2029]" },
                peg$c16 = { type: "other", description: "end of line" },
                peg$c17 = "\n",
                peg$c18 = { type: "literal", value: "\n", description: "\"\\n\"" },
                peg$c19 = "\r\n",
                peg$c20 = { type: "literal", value: "\r\n", description: "\"\\r\\n\"" },
                peg$c21 = "\r",
                peg$c22 = { type: "literal", value: "\r", description: "\"\\r\"" },
                peg$c23 = "\u2028",
                peg$c24 = { type: "literal", value: "\u2028", description: "\"\\u2028\"" },
                peg$c25 = "\u2029",
                peg$c26 = { type: "literal", value: "\u2029", description: "\"\\u2029\"" },
                peg$c27 = ";",
                peg$c28 = { type: "literal", value: ";", description: "\";\"" },
                peg$c29 = void 0,
                peg$c30 = { type: "other", description: "comment" },
                peg$c31 = "/*",
                peg$c32 = { type: "literal", value: "/*", description: "\"/*\"" },
                peg$c33 = "*/",
                peg$c34 = { type: "literal", value: "*/", description: "\"*/\"" },
                peg$c35 = "//",
                peg$c36 = { type: "literal", value: "//", description: "\"//\"" },
                peg$c37 = function(val) {
                    return {
                        type: "NumericLiteral",
                        value: val
                    }
                },
                peg$c38 = /^[0-9]/,
                peg$c39 = { type: "class", value: "[0-9]", description: "[0-9]" },
                peg$c40 = function(digits) {
                    return parseInt(digits.join(""));
                },
                peg$c41 = ".",
                peg$c42 = { type: "literal", value: ".", description: "\".\"" },
                peg$c43 = function(digits) {
                    return parseFloat(digits.join(""));
                },
                peg$c44 = null,
                peg$c45 = /^[\-+]/,
                peg$c46 = { type: "class", value: "[\\-+]", description: "[\\-+]" },
                peg$c47 = { type: "other", description: "identifier" },
                peg$c48 = function(name) { return name; },
                peg$c49 = function(start, parts) {
                    return start + parts.join("");
                },
                peg$c50 = function(name) { return { type: "Variable", name: name }; },
                peg$c51 = "(",
                peg$c52 = { type: "literal", value: "(", description: "\"(\"" },
                peg$c53 = ")",
                peg$c54 = { type: "literal", value: ")", description: "\")\"" },
                peg$c55 = function(operator, expression) {
                    return {
                        type:       "UnaryExpression",
                        operator:   operator,
                        expression: expression
                    };
                },
                peg$c56 = "+",
                peg$c57 = { type: "literal", value: "+", description: "\"+\"" },
                peg$c58 = "-",
                peg$c59 = { type: "literal", value: "-", description: "\"-\"" },
                peg$c60 = "!",
                peg$c61 = { type: "literal", value: "!", description: "\"!\"" },
                peg$c62 = function(head, tail) {
                    var result = head;
                    for (var i = 0; i < tail.length; i++) {
                        result = {
                            type:     "MultiplicativeExpression",
                            operator: tail[i][1],
                            left:     result,
                            right:    tail[i][3]
                        };
                    }
                    return result;
                },
                peg$c63 = "*",
                peg$c64 = { type: "literal", value: "*", description: "\"*\"" },
                peg$c65 = "/",
                peg$c66 = { type: "literal", value: "/", description: "\"/\"" },
                peg$c67 = function(head, tail) {
                    var result = head;
                    for (var i = 0; i < tail.length; i++) {
                        result = {
                            type:     "AdditiveExpression",
                            operator: tail[i][1],
                            left:     result,
                            right:    tail[i][3]
                        };
                    }
                    return result;
                },
                peg$c68 = function(head, tail) {
                    var result = head;
                    for (var i = 0; i < tail.length; i++) {
                        result = {
                            type:     "Inequality",
                            operator: tail[i][1],
                            left:     result,
                            right:    tail[i][3]
                        };
                    }
                    return result;
                },
                peg$c69 = "<=",
                peg$c70 = { type: "literal", value: "<=", description: "\"<=\"" },
                peg$c71 = ">=",
                peg$c72 = { type: "literal", value: ">=", description: "\">=\"" },
                peg$c73 = "<",
                peg$c74 = { type: "literal", value: "<", description: "\"<\"" },
                peg$c75 = ">",
                peg$c76 = { type: "literal", value: ">", description: "\">\"" },
                peg$c77 = "==",
                peg$c78 = { type: "literal", value: "==", description: "\"==\"" },
                peg$c79 = function(head, tail) {
                    var result = head;
                    for (var i = 0; i < tail.length; i++) {
                        result = {
                            type:     "Equality",
                            operator: tail[i][1],
                            left:     result,
                            right:    tail[i][3]
                        };
                    }
                    return result;
                },

                peg$currPos          = 0,
                peg$reportedPos      = 0,
                peg$cachedPos        = 0,
                peg$cachedPosDetails = { line: 1, column: 1, seenCR: false },
                peg$maxFailPos       = 0,
                peg$maxFailExpected  = [],
                peg$silentFails      = 0,

                peg$result;

            if ("startRule" in options) {
                if (!(options.startRule in peg$startRuleFunctions)) {
                    throw new Error("Can't start parsing from rule \"" + options.startRule + "\".");
                }

                peg$startRuleFunction = peg$startRuleFunctions[options.startRule];
            }

            function text() {
                return input.substring(peg$reportedPos, peg$currPos);
            }

            function offset() {
                return peg$reportedPos;
            }

            function line() {
                return peg$computePosDetails(peg$reportedPos).line;
            }

            function column() {
                return peg$computePosDetails(peg$reportedPos).column;
            }

            function expected(description) {
                throw peg$buildException(
                    null,
                    [{ type: "other", description: description }],
                    peg$reportedPos
                );
            }

            function error(message) {
                throw peg$buildException(message, null, peg$reportedPos);
            }

            function peg$computePosDetails(pos) {
                function advance(details, startPos, endPos) {
                    var p, ch;

                    for (p = startPos; p < endPos; p++) {
                        ch = input.charAt(p);
                        if (ch === "\n") {
                            if (!details.seenCR) { details.line++; }
                            details.column = 1;
                            details.seenCR = false;
                        } else if (ch === "\r" || ch === "\u2028" || ch === "\u2029") {
                            details.line++;
                            details.column = 1;
                            details.seenCR = true;
                        } else {
                            details.column++;
                            details.seenCR = false;
                        }
                    }
                }

                if (peg$cachedPos !== pos) {
                    if (peg$cachedPos > pos) {
                        peg$cachedPos = 0;
                        peg$cachedPosDetails = { line: 1, column: 1, seenCR: false };
                    }
                    advance(peg$cachedPosDetails, peg$cachedPos, pos);
                    peg$cachedPos = pos;
                }

                return peg$cachedPosDetails;
            }

            function peg$fail(expected) {
                if (peg$currPos < peg$maxFailPos) { return; }

                if (peg$currPos > peg$maxFailPos) {
                    peg$maxFailPos = peg$currPos;
                    peg$maxFailExpected = [];
                }

                peg$maxFailExpected.push(expected);
            }

            function peg$buildException(message, expected, pos) {
                function cleanupExpected(expected) {
                    var i = 1;

                    expected.sort(function(a, b) {
                        if (a.description < b.description) {
                            return -1;
                        } else if (a.description > b.description) {
                            return 1;
                        } else {
                            return 0;
                        }
                    });

                    while (i < expected.length) {
                        if (expected[i - 1] === expected[i]) {
                            expected.splice(i, 1);
                        } else {
                            i++;
                        }
                    }
                }

                function buildMessage(expected, found) {
                    function stringEscape(s) {
                        function hex(ch) { return ch.charCodeAt(0).toString(16).toUpperCase(); }

                        return s
                            .replace(/\\/g,   '\\\\')
                            .replace(/"/g,    '\\"')
                            .replace(/\x08/g, '\\b')
                            .replace(/\t/g,   '\\t')
                            .replace(/\n/g,   '\\n')
                            .replace(/\f/g,   '\\f')
                            .replace(/\r/g,   '\\r')
                            .replace(/[\x00-\x07\x0B\x0E\x0F]/g, function(ch) { return '\\x0' + hex(ch); })
                            .replace(/[\x10-\x1F\x80-\xFF]/g,    function(ch) { return '\\x'  + hex(ch); })
                            .replace(/[\u0180-\u0FFF]/g,         function(ch) { return '\\u0' + hex(ch); })
                            .replace(/[\u1080-\uFFFF]/g,         function(ch) { return '\\u'  + hex(ch); });
                    }

                    var expectedDescs = new Array(expected.length),
                        expectedDesc, foundDesc, i;

                    for (i = 0; i < expected.length; i++) {
                        expectedDescs[i] = expected[i].description;
                    }

                    expectedDesc = expected.length > 1
                        ? expectedDescs.slice(0, -1).join(", ")
                        + " or "
                        + expectedDescs[expected.length - 1]
                        : expectedDescs[0];

                    foundDesc = found ? "\"" + stringEscape(found) + "\"" : "end of input";

                    return "Expected " + expectedDesc + " but " + foundDesc + " found.";
                }

                var posDetails = peg$computePosDetails(pos),
                    found      = pos < input.length ? input.charAt(pos) : null;

                if (expected !== null) {
                    cleanupExpected(expected);
                }

                return new SyntaxError(
                    message !== null ? message : buildMessage(expected, found),
                    expected,
                    found,
                    pos,
                    posDetails.line,
                    posDetails.column
                );
            }

            function peg$parsestart() {
                var s0, s1, s2, s3;

                s0 = peg$currPos;
                s1 = peg$parse__();
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$parseStatement();
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$parseStatement();
                    }
                    if (s2 !== peg$FAILED) {
                        s3 = peg$parse__();
                        if (s3 !== peg$FAILED) {
                            peg$reportedPos = s0;
                            s1 = peg$c2(s2);
                            s0 = s1;
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseStatement() {
                var s0, s1, s2;

                s0 = peg$currPos;
                s1 = peg$parseLinearExpression();
                if (s1 !== peg$FAILED) {
                    s2 = peg$parseEOS();
                    if (s2 !== peg$FAILED) {
                        peg$reportedPos = s0;
                        s1 = peg$c3(s1);
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseSourceCharacter() {
                var s0;

                if (input.length > peg$currPos) {
                    s0 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c4); }
                }

                return s0;
            }

            function peg$parseIdentifierStart() {
                var s0;

                if (peg$c5.test(input.charAt(peg$currPos))) {
                    s0 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c6); }
                }
                if (s0 === peg$FAILED) {
                    if (input.charCodeAt(peg$currPos) === 36) {
                        s0 = peg$c7;
                        peg$currPos++;
                    } else {
                        s0 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c8); }
                    }
                    if (s0 === peg$FAILED) {
                        if (input.charCodeAt(peg$currPos) === 95) {
                            s0 = peg$c9;
                            peg$currPos++;
                        } else {
                            s0 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c10); }
                        }
                    }
                }

                return s0;
            }

            function peg$parseWhiteSpace() {
                var s0, s1;

                peg$silentFails++;
                if (peg$c12.test(input.charAt(peg$currPos))) {
                    s0 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c13); }
                }
                peg$silentFails--;
                if (s0 === peg$FAILED) {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c11); }
                }

                return s0;
            }

            function peg$parseLineTerminator() {
                var s0;

                if (peg$c14.test(input.charAt(peg$currPos))) {
                    s0 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c15); }
                }

                return s0;
            }

            function peg$parseLineTerminatorSequence() {
                var s0, s1;

                peg$silentFails++;
                if (input.charCodeAt(peg$currPos) === 10) {
                    s0 = peg$c17;
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c18); }
                }
                if (s0 === peg$FAILED) {
                    if (input.substr(peg$currPos, 2) === peg$c19) {
                        s0 = peg$c19;
                        peg$currPos += 2;
                    } else {
                        s0 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c20); }
                    }
                    if (s0 === peg$FAILED) {
                        if (input.charCodeAt(peg$currPos) === 13) {
                            s0 = peg$c21;
                            peg$currPos++;
                        } else {
                            s0 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c22); }
                        }
                        if (s0 === peg$FAILED) {
                            if (input.charCodeAt(peg$currPos) === 8232) {
                                s0 = peg$c23;
                                peg$currPos++;
                            } else {
                                s0 = peg$FAILED;
                                if (peg$silentFails === 0) { peg$fail(peg$c24); }
                            }
                            if (s0 === peg$FAILED) {
                                if (input.charCodeAt(peg$currPos) === 8233) {
                                    s0 = peg$c25;
                                    peg$currPos++;
                                } else {
                                    s0 = peg$FAILED;
                                    if (peg$silentFails === 0) { peg$fail(peg$c26); }
                                }
                            }
                        }
                    }
                }
                peg$silentFails--;
                if (s0 === peg$FAILED) {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c16); }
                }

                return s0;
            }

            function peg$parseEOS() {
                var s0, s1, s2;

                s0 = peg$currPos;
                s1 = peg$parse__();
                if (s1 !== peg$FAILED) {
                    if (input.charCodeAt(peg$currPos) === 59) {
                        s2 = peg$c27;
                        peg$currPos++;
                    } else {
                        s2 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c28); }
                    }
                    if (s2 !== peg$FAILED) {
                        s1 = [s1, s2];
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }
                if (s0 === peg$FAILED) {
                    s0 = peg$currPos;
                    s1 = peg$parse_();
                    if (s1 !== peg$FAILED) {
                        s2 = peg$parseLineTerminatorSequence();
                        if (s2 !== peg$FAILED) {
                            s1 = [s1, s2];
                            s0 = s1;
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                    if (s0 === peg$FAILED) {
                        s0 = peg$currPos;
                        s1 = peg$parse__();
                        if (s1 !== peg$FAILED) {
                            s2 = peg$parseEOF();
                            if (s2 !== peg$FAILED) {
                                s1 = [s1, s2];
                                s0 = s1;
                            } else {
                                peg$currPos = s0;
                                s0 = peg$c0;
                            }
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    }
                }

                return s0;
            }

            function peg$parseEOF() {
                var s0, s1;

                s0 = peg$currPos;
                peg$silentFails++;
                if (input.length > peg$currPos) {
                    s1 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c4); }
                }
                peg$silentFails--;
                if (s1 === peg$FAILED) {
                    s0 = peg$c29;
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseComment() {
                var s0, s1;

                peg$silentFails++;
                s0 = peg$parseMultiLineComment();
                if (s0 === peg$FAILED) {
                    s0 = peg$parseSingleLineComment();
                }
                peg$silentFails--;
                if (s0 === peg$FAILED) {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c30); }
                }

                return s0;
            }

            function peg$parseMultiLineComment() {
                var s0, s1, s2, s3, s4, s5;

                s0 = peg$currPos;
                if (input.substr(peg$currPos, 2) === peg$c31) {
                    s1 = peg$c31;
                    peg$currPos += 2;
                } else {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c32); }
                }
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$currPos;
                    peg$silentFails++;
                    if (input.substr(peg$currPos, 2) === peg$c33) {
                        s5 = peg$c33;
                        peg$currPos += 2;
                    } else {
                        s5 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c34); }
                    }
                    peg$silentFails--;
                    if (s5 === peg$FAILED) {
                        s4 = peg$c29;
                    } else {
                        peg$currPos = s4;
                        s4 = peg$c0;
                    }
                    if (s4 !== peg$FAILED) {
                        s5 = peg$parseSourceCharacter();
                        if (s5 !== peg$FAILED) {
                            s4 = [s4, s5];
                            s3 = s4;
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$currPos;
                        peg$silentFails++;
                        if (input.substr(peg$currPos, 2) === peg$c33) {
                            s5 = peg$c33;
                            peg$currPos += 2;
                        } else {
                            s5 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c34); }
                        }
                        peg$silentFails--;
                        if (s5 === peg$FAILED) {
                            s4 = peg$c29;
                        } else {
                            peg$currPos = s4;
                            s4 = peg$c0;
                        }
                        if (s4 !== peg$FAILED) {
                            s5 = peg$parseSourceCharacter();
                            if (s5 !== peg$FAILED) {
                                s4 = [s4, s5];
                                s3 = s4;
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        if (input.substr(peg$currPos, 2) === peg$c33) {
                            s3 = peg$c33;
                            peg$currPos += 2;
                        } else {
                            s3 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c34); }
                        }
                        if (s3 !== peg$FAILED) {
                            s1 = [s1, s2, s3];
                            s0 = s1;
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseMultiLineCommentNoLineTerminator() {
                var s0, s1, s2, s3, s4, s5;

                s0 = peg$currPos;
                if (input.substr(peg$currPos, 2) === peg$c31) {
                    s1 = peg$c31;
                    peg$currPos += 2;
                } else {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c32); }
                }
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$currPos;
                    peg$silentFails++;
                    if (input.substr(peg$currPos, 2) === peg$c33) {
                        s5 = peg$c33;
                        peg$currPos += 2;
                    } else {
                        s5 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c34); }
                    }
                    if (s5 === peg$FAILED) {
                        s5 = peg$parseLineTerminator();
                    }
                    peg$silentFails--;
                    if (s5 === peg$FAILED) {
                        s4 = peg$c29;
                    } else {
                        peg$currPos = s4;
                        s4 = peg$c0;
                    }
                    if (s4 !== peg$FAILED) {
                        s5 = peg$parseSourceCharacter();
                        if (s5 !== peg$FAILED) {
                            s4 = [s4, s5];
                            s3 = s4;
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$currPos;
                        peg$silentFails++;
                        if (input.substr(peg$currPos, 2) === peg$c33) {
                            s5 = peg$c33;
                            peg$currPos += 2;
                        } else {
                            s5 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c34); }
                        }
                        if (s5 === peg$FAILED) {
                            s5 = peg$parseLineTerminator();
                        }
                        peg$silentFails--;
                        if (s5 === peg$FAILED) {
                            s4 = peg$c29;
                        } else {
                            peg$currPos = s4;
                            s4 = peg$c0;
                        }
                        if (s4 !== peg$FAILED) {
                            s5 = peg$parseSourceCharacter();
                            if (s5 !== peg$FAILED) {
                                s4 = [s4, s5];
                                s3 = s4;
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        if (input.substr(peg$currPos, 2) === peg$c33) {
                            s3 = peg$c33;
                            peg$currPos += 2;
                        } else {
                            s3 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c34); }
                        }
                        if (s3 !== peg$FAILED) {
                            s1 = [s1, s2, s3];
                            s0 = s1;
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseSingleLineComment() {
                var s0, s1, s2, s3, s4, s5;

                s0 = peg$currPos;
                if (input.substr(peg$currPos, 2) === peg$c35) {
                    s1 = peg$c35;
                    peg$currPos += 2;
                } else {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c36); }
                }
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$currPos;
                    peg$silentFails++;
                    s5 = peg$parseLineTerminator();
                    peg$silentFails--;
                    if (s5 === peg$FAILED) {
                        s4 = peg$c29;
                    } else {
                        peg$currPos = s4;
                        s4 = peg$c0;
                    }
                    if (s4 !== peg$FAILED) {
                        s5 = peg$parseSourceCharacter();
                        if (s5 !== peg$FAILED) {
                            s4 = [s4, s5];
                            s3 = s4;
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$currPos;
                        peg$silentFails++;
                        s5 = peg$parseLineTerminator();
                        peg$silentFails--;
                        if (s5 === peg$FAILED) {
                            s4 = peg$c29;
                        } else {
                            peg$currPos = s4;
                            s4 = peg$c0;
                        }
                        if (s4 !== peg$FAILED) {
                            s5 = peg$parseSourceCharacter();
                            if (s5 !== peg$FAILED) {
                                s4 = [s4, s5];
                                s3 = s4;
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        s3 = peg$parseLineTerminator();
                        if (s3 === peg$FAILED) {
                            s3 = peg$parseEOF();
                        }
                        if (s3 !== peg$FAILED) {
                            s1 = [s1, s2, s3];
                            s0 = s1;
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parse_() {
                var s0, s1;

                s0 = [];
                s1 = peg$parseWhiteSpace();
                if (s1 === peg$FAILED) {
                    s1 = peg$parseMultiLineCommentNoLineTerminator();
                    if (s1 === peg$FAILED) {
                        s1 = peg$parseSingleLineComment();
                    }
                }
                while (s1 !== peg$FAILED) {
                    s0.push(s1);
                    s1 = peg$parseWhiteSpace();
                    if (s1 === peg$FAILED) {
                        s1 = peg$parseMultiLineCommentNoLineTerminator();
                        if (s1 === peg$FAILED) {
                            s1 = peg$parseSingleLineComment();
                        }
                    }
                }

                return s0;
            }

            function peg$parse__() {
                var s0, s1;

                s0 = [];
                s1 = peg$parseWhiteSpace();
                if (s1 === peg$FAILED) {
                    s1 = peg$parseLineTerminatorSequence();
                    if (s1 === peg$FAILED) {
                        s1 = peg$parseComment();
                    }
                }
                while (s1 !== peg$FAILED) {
                    s0.push(s1);
                    s1 = peg$parseWhiteSpace();
                    if (s1 === peg$FAILED) {
                        s1 = peg$parseLineTerminatorSequence();
                        if (s1 === peg$FAILED) {
                            s1 = peg$parseComment();
                        }
                    }
                }

                return s0;
            }

            function peg$parseLiteral() {
                var s0, s1;

                s0 = peg$currPos;
                s1 = peg$parseReal();
                if (s1 === peg$FAILED) {
                    s1 = peg$parseInteger();
                }
                if (s1 !== peg$FAILED) {
                    peg$reportedPos = s0;
                    s1 = peg$c37(s1);
                }
                s0 = s1;

                return s0;
            }

            function peg$parseInteger() {
                var s0, s1, s2;

                s0 = peg$currPos;
                s1 = [];
                if (peg$c38.test(input.charAt(peg$currPos))) {
                    s2 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s2 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c39); }
                }
                if (s2 !== peg$FAILED) {
                    while (s2 !== peg$FAILED) {
                        s1.push(s2);
                        if (peg$c38.test(input.charAt(peg$currPos))) {
                            s2 = input.charAt(peg$currPos);
                            peg$currPos++;
                        } else {
                            s2 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c39); }
                        }
                    }
                } else {
                    s1 = peg$c0;
                }
                if (s1 !== peg$FAILED) {
                    peg$reportedPos = s0;
                    s1 = peg$c40(s1);
                }
                s0 = s1;

                return s0;
            }

            function peg$parseReal() {
                var s0, s1, s2, s3, s4;

                s0 = peg$currPos;
                s1 = peg$currPos;
                s2 = peg$parseInteger();
                if (s2 !== peg$FAILED) {
                    if (input.charCodeAt(peg$currPos) === 46) {
                        s3 = peg$c41;
                        peg$currPos++;
                    } else {
                        s3 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c42); }
                    }
                    if (s3 !== peg$FAILED) {
                        s4 = peg$parseInteger();
                        if (s4 !== peg$FAILED) {
                            s2 = [s2, s3, s4];
                            s1 = s2;
                        } else {
                            peg$currPos = s1;
                            s1 = peg$c0;
                        }
                    } else {
                        peg$currPos = s1;
                        s1 = peg$c0;
                    }
                } else {
                    peg$currPos = s1;
                    s1 = peg$c0;
                }
                if (s1 !== peg$FAILED) {
                    peg$reportedPos = s0;
                    s1 = peg$c43(s1);
                }
                s0 = s1;

                return s0;
            }

            function peg$parseSignedInteger() {
                var s0, s1, s2, s3;

                s0 = peg$currPos;
                if (peg$c45.test(input.charAt(peg$currPos))) {
                    s1 = input.charAt(peg$currPos);
                    peg$currPos++;
                } else {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c46); }
                }
                if (s1 === peg$FAILED) {
                    s1 = peg$c44;
                }
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    if (peg$c38.test(input.charAt(peg$currPos))) {
                        s3 = input.charAt(peg$currPos);
                        peg$currPos++;
                    } else {
                        s3 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c39); }
                    }
                    if (s3 !== peg$FAILED) {
                        while (s3 !== peg$FAILED) {
                            s2.push(s3);
                            if (peg$c38.test(input.charAt(peg$currPos))) {
                                s3 = input.charAt(peg$currPos);
                                peg$currPos++;
                            } else {
                                s3 = peg$FAILED;
                                if (peg$silentFails === 0) { peg$fail(peg$c39); }
                            }
                        }
                    } else {
                        s2 = peg$c0;
                    }
                    if (s2 !== peg$FAILED) {
                        s1 = [s1, s2];
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseIdentifier() {
                var s0, s1;

                peg$silentFails++;
                s0 = peg$currPos;
                s1 = peg$parseIdentifierName();
                if (s1 !== peg$FAILED) {
                    peg$reportedPos = s0;
                    s1 = peg$c48(s1);
                }
                s0 = s1;
                peg$silentFails--;
                if (s0 === peg$FAILED) {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c47); }
                }

                return s0;
            }

            function peg$parseIdentifierName() {
                var s0, s1, s2, s3;

                peg$silentFails++;
                s0 = peg$currPos;
                s1 = peg$parseIdentifierStart();
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$parseIdentifierStart();
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$parseIdentifierStart();
                    }
                    if (s2 !== peg$FAILED) {
                        peg$reportedPos = s0;
                        s1 = peg$c49(s1, s2);
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }
                peg$silentFails--;
                if (s0 === peg$FAILED) {
                    s1 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c47); }
                }

                return s0;
            }

            function peg$parsePrimaryExpression() {
                var s0, s1, s2, s3, s4, s5;

                s0 = peg$currPos;
                s1 = peg$parseIdentifier();
                if (s1 !== peg$FAILED) {
                    peg$reportedPos = s0;
                    s1 = peg$c50(s1);
                }
                s0 = s1;
                if (s0 === peg$FAILED) {
                    s0 = peg$parseLiteral();
                    if (s0 === peg$FAILED) {
                        s0 = peg$currPos;
                        if (input.charCodeAt(peg$currPos) === 40) {
                            s1 = peg$c51;
                            peg$currPos++;
                        } else {
                            s1 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c52); }
                        }
                        if (s1 !== peg$FAILED) {
                            s2 = peg$parse__();
                            if (s2 !== peg$FAILED) {
                                s3 = peg$parseLinearExpression();
                                if (s3 !== peg$FAILED) {
                                    s4 = peg$parse__();
                                    if (s4 !== peg$FAILED) {
                                        if (input.charCodeAt(peg$currPos) === 41) {
                                            s5 = peg$c53;
                                            peg$currPos++;
                                        } else {
                                            s5 = peg$FAILED;
                                            if (peg$silentFails === 0) { peg$fail(peg$c54); }
                                        }
                                        if (s5 !== peg$FAILED) {
                                            peg$reportedPos = s0;
                                            s1 = peg$c3(s3);
                                            s0 = s1;
                                        } else {
                                            peg$currPos = s0;
                                            s0 = peg$c0;
                                        }
                                    } else {
                                        peg$currPos = s0;
                                        s0 = peg$c0;
                                    }
                                } else {
                                    peg$currPos = s0;
                                    s0 = peg$c0;
                                }
                            } else {
                                peg$currPos = s0;
                                s0 = peg$c0;
                            }
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    }
                }

                return s0;
            }

            function peg$parseUnaryExpression() {
                var s0, s1, s2, s3;

                s0 = peg$parsePrimaryExpression();
                if (s0 === peg$FAILED) {
                    s0 = peg$currPos;
                    s1 = peg$parseUnaryOperator();
                    if (s1 !== peg$FAILED) {
                        s2 = peg$parse__();
                        if (s2 !== peg$FAILED) {
                            s3 = peg$parseUnaryExpression();
                            if (s3 !== peg$FAILED) {
                                peg$reportedPos = s0;
                                s1 = peg$c55(s1, s3);
                                s0 = s1;
                            } else {
                                peg$currPos = s0;
                                s0 = peg$c0;
                            }
                        } else {
                            peg$currPos = s0;
                            s0 = peg$c0;
                        }
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                }

                return s0;
            }

            function peg$parseUnaryOperator() {
                var s0;

                if (input.charCodeAt(peg$currPos) === 43) {
                    s0 = peg$c56;
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c57); }
                }
                if (s0 === peg$FAILED) {
                    if (input.charCodeAt(peg$currPos) === 45) {
                        s0 = peg$c58;
                        peg$currPos++;
                    } else {
                        s0 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c59); }
                    }
                    if (s0 === peg$FAILED) {
                        if (input.charCodeAt(peg$currPos) === 33) {
                            s0 = peg$c60;
                            peg$currPos++;
                        } else {
                            s0 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c61); }
                        }
                    }
                }

                return s0;
            }

            function peg$parseMultiplicativeExpression() {
                var s0, s1, s2, s3, s4, s5, s6, s7;

                s0 = peg$currPos;
                s1 = peg$parseUnaryExpression();
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$parse__();
                    if (s4 !== peg$FAILED) {
                        s5 = peg$parseMultiplicativeOperator();
                        if (s5 !== peg$FAILED) {
                            s6 = peg$parse__();
                            if (s6 !== peg$FAILED) {
                                s7 = peg$parseUnaryExpression();
                                if (s7 !== peg$FAILED) {
                                    s4 = [s4, s5, s6, s7];
                                    s3 = s4;
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$parse__();
                        if (s4 !== peg$FAILED) {
                            s5 = peg$parseMultiplicativeOperator();
                            if (s5 !== peg$FAILED) {
                                s6 = peg$parse__();
                                if (s6 !== peg$FAILED) {
                                    s7 = peg$parseUnaryExpression();
                                    if (s7 !== peg$FAILED) {
                                        s4 = [s4, s5, s6, s7];
                                        s3 = s4;
                                    } else {
                                        peg$currPos = s3;
                                        s3 = peg$c0;
                                    }
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        peg$reportedPos = s0;
                        s1 = peg$c62(s1, s2);
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseMultiplicativeOperator() {
                var s0;

                if (input.charCodeAt(peg$currPos) === 42) {
                    s0 = peg$c63;
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c64); }
                }
                if (s0 === peg$FAILED) {
                    if (input.charCodeAt(peg$currPos) === 47) {
                        s0 = peg$c65;
                        peg$currPos++;
                    } else {
                        s0 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c66); }
                    }
                }

                return s0;
            }

            function peg$parseAdditiveExpression() {
                var s0, s1, s2, s3, s4, s5, s6, s7;

                s0 = peg$currPos;
                s1 = peg$parseMultiplicativeExpression();
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$parse__();
                    if (s4 !== peg$FAILED) {
                        s5 = peg$parseAdditiveOperator();
                        if (s5 !== peg$FAILED) {
                            s6 = peg$parse__();
                            if (s6 !== peg$FAILED) {
                                s7 = peg$parseMultiplicativeExpression();
                                if (s7 !== peg$FAILED) {
                                    s4 = [s4, s5, s6, s7];
                                    s3 = s4;
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$parse__();
                        if (s4 !== peg$FAILED) {
                            s5 = peg$parseAdditiveOperator();
                            if (s5 !== peg$FAILED) {
                                s6 = peg$parse__();
                                if (s6 !== peg$FAILED) {
                                    s7 = peg$parseMultiplicativeExpression();
                                    if (s7 !== peg$FAILED) {
                                        s4 = [s4, s5, s6, s7];
                                        s3 = s4;
                                    } else {
                                        peg$currPos = s3;
                                        s3 = peg$c0;
                                    }
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        peg$reportedPos = s0;
                        s1 = peg$c67(s1, s2);
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseAdditiveOperator() {
                var s0;

                if (input.charCodeAt(peg$currPos) === 43) {
                    s0 = peg$c56;
                    peg$currPos++;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c57); }
                }
                if (s0 === peg$FAILED) {
                    if (input.charCodeAt(peg$currPos) === 45) {
                        s0 = peg$c58;
                        peg$currPos++;
                    } else {
                        s0 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c59); }
                    }
                }

                return s0;
            }

            function peg$parseInequalityExpression() {
                var s0, s1, s2, s3, s4, s5, s6, s7;

                s0 = peg$currPos;
                s1 = peg$parseAdditiveExpression();
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$parse__();
                    if (s4 !== peg$FAILED) {
                        s5 = peg$parseInequalityOperator();
                        if (s5 !== peg$FAILED) {
                            s6 = peg$parse__();
                            if (s6 !== peg$FAILED) {
                                s7 = peg$parseAdditiveExpression();
                                if (s7 !== peg$FAILED) {
                                    s4 = [s4, s5, s6, s7];
                                    s3 = s4;
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$parse__();
                        if (s4 !== peg$FAILED) {
                            s5 = peg$parseInequalityOperator();
                            if (s5 !== peg$FAILED) {
                                s6 = peg$parse__();
                                if (s6 !== peg$FAILED) {
                                    s7 = peg$parseAdditiveExpression();
                                    if (s7 !== peg$FAILED) {
                                        s4 = [s4, s5, s6, s7];
                                        s3 = s4;
                                    } else {
                                        peg$currPos = s3;
                                        s3 = peg$c0;
                                    }
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        peg$reportedPos = s0;
                        s1 = peg$c68(s1, s2);
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            function peg$parseInequalityOperator() {
                var s0;

                if (input.substr(peg$currPos, 2) === peg$c69) {
                    s0 = peg$c69;
                    peg$currPos += 2;
                } else {
                    s0 = peg$FAILED;
                    if (peg$silentFails === 0) { peg$fail(peg$c70); }
                }
                if (s0 === peg$FAILED) {
                    if (input.substr(peg$currPos, 2) === peg$c71) {
                        s0 = peg$c71;
                        peg$currPos += 2;
                    } else {
                        s0 = peg$FAILED;
                        if (peg$silentFails === 0) { peg$fail(peg$c72); }
                    }
                    if (s0 === peg$FAILED) {
                        if (input.charCodeAt(peg$currPos) === 60) {
                            s0 = peg$c73;
                            peg$currPos++;
                        } else {
                            s0 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c74); }
                        }
                        if (s0 === peg$FAILED) {
                            if (input.charCodeAt(peg$currPos) === 62) {
                                s0 = peg$c75;
                                peg$currPos++;
                            } else {
                                s0 = peg$FAILED;
                                if (peg$silentFails === 0) { peg$fail(peg$c76); }
                            }
                        }
                    }
                }

                return s0;
            }

            function peg$parseLinearExpression() {
                var s0, s1, s2, s3, s4, s5, s6, s7;

                s0 = peg$currPos;
                s1 = peg$parseInequalityExpression();
                if (s1 !== peg$FAILED) {
                    s2 = [];
                    s3 = peg$currPos;
                    s4 = peg$parse__();
                    if (s4 !== peg$FAILED) {
                        if (input.substr(peg$currPos, 2) === peg$c77) {
                            s5 = peg$c77;
                            peg$currPos += 2;
                        } else {
                            s5 = peg$FAILED;
                            if (peg$silentFails === 0) { peg$fail(peg$c78); }
                        }
                        if (s5 !== peg$FAILED) {
                            s6 = peg$parse__();
                            if (s6 !== peg$FAILED) {
                                s7 = peg$parseInequalityExpression();
                                if (s7 !== peg$FAILED) {
                                    s4 = [s4, s5, s6, s7];
                                    s3 = s4;
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    } else {
                        peg$currPos = s3;
                        s3 = peg$c0;
                    }
                    while (s3 !== peg$FAILED) {
                        s2.push(s3);
                        s3 = peg$currPos;
                        s4 = peg$parse__();
                        if (s4 !== peg$FAILED) {
                            if (input.substr(peg$currPos, 2) === peg$c77) {
                                s5 = peg$c77;
                                peg$currPos += 2;
                            } else {
                                s5 = peg$FAILED;
                                if (peg$silentFails === 0) { peg$fail(peg$c78); }
                            }
                            if (s5 !== peg$FAILED) {
                                s6 = peg$parse__();
                                if (s6 !== peg$FAILED) {
                                    s7 = peg$parseInequalityExpression();
                                    if (s7 !== peg$FAILED) {
                                        s4 = [s4, s5, s6, s7];
                                        s3 = s4;
                                    } else {
                                        peg$currPos = s3;
                                        s3 = peg$c0;
                                    }
                                } else {
                                    peg$currPos = s3;
                                    s3 = peg$c0;
                                }
                            } else {
                                peg$currPos = s3;
                                s3 = peg$c0;
                            }
                        } else {
                            peg$currPos = s3;
                            s3 = peg$c0;
                        }
                    }
                    if (s2 !== peg$FAILED) {
                        peg$reportedPos = s0;
                        s1 = peg$c79(s1, s2);
                        s0 = s1;
                    } else {
                        peg$currPos = s0;
                        s0 = peg$c0;
                    }
                } else {
                    peg$currPos = s0;
                    s0 = peg$c0;
                }

                return s0;
            }

            peg$result = peg$startRuleFunction();

            if (peg$result !== peg$FAILED && peg$currPos === input.length) {
                return peg$result;
            } else {
                if (peg$result !== peg$FAILED && peg$currPos < input.length) {
                    peg$fail({ type: "end", description: "end of input" });
                }

                throw peg$buildException(null, peg$maxFailExpected, peg$maxFailPos);
            }
        }

        return {
            SyntaxError: SyntaxError,
            parse:       parse
        };
    })();
    // Copyright (C) 2013, Alex Russell <slightlyoff@chromium.org>
    // Use of this source code is governed by
    //    http://www.apache.org/licenses/LICENSE-2.0

    (function(c){
        "use strict";

        var solver = new c.SimplexSolver();
        var vars = {};
        var exprs = {};

        var weak = c.Strength.weak;
        var medium = c.Strength.medium;
        var strong = c.Strength.strong;
        var required = c.Strength.required;

        var _c = function(expr) {
            if (exprs[expr]) {
                return exprs[expr];
            }
            switch(expr.type) {
            case "Inequality":
                var op = (expr.operator == "<=") ? c.LEQ : c.GEQ;
                var i = new c.Inequality(_c(expr.left), op, _c(expr.right), weak);
                solver.addConstraint(i);
                return i;
            case "Equality":
                var i = new c.Equation(_c(expr.left), _c(expr.right), weak);
                solver.addConstraint(i);
                return i;
            case "MultiplicativeExpression":
                var i = c.times(_c(expr.left), _c(expr.right));
                solver.addConstraint(i);
                return i;
            case "AdditiveExpression":
                if (expr.operator == "+") {
                    return c.plus(_c(expr.left), _c(expr.right));
                } else {
                    return c.minus(_c(expr.left), _c(expr.right));
                }
            case "NumericLiteral":
                return new c.Expression(expr.value);
            case "Variable":
                // console.log(expr);
                if(!vars[expr.name]) {
                    vars[expr.name] = new c.Variable({ name: expr.name });
                }
                return vars[expr.name];
            case "UnaryExpression":
                console.log("UnaryExpression...WTF?");
                break;
            }
        };

        var compile = function(expressions) {
            return expressions.map(_c);
        };

        // Global API entrypoint
        c._api = function() {
            var args = Array.prototype.slice.call(arguments);
            if (args.length == 1) {
                if(typeof args[0] == "string") {
                    // Parse and execute it
                    var r = c.parser.parse(args[0]);
                    return compile(r);
                } else if(typeof args[0] == "function") {
                    solver._addCallback(args[0]);
                }
            }
        };

    })(this["c"]||module.parent.exports||{});
}).call(
    (typeof module != "undefined") ?
        (module.compiled = true && module) : cassowaryScope
);

var c = cassowaryScope.c

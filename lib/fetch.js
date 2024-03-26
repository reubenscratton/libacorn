
globalThis.Headers = class {
  map = new Map(); // key is lower-cased header name, value is array of strings
  constructor(init) {
    if (!init) return;
    if (typeof(init) === 'object') {
      // an array of name-val pairs
      if (Array.isArray(init)) {
          for (const e of init) {
            this.append(e[0], e[1]);
          }
      }
      // another Headers instance
      else if (init.constructor === Headers) {
        this.map = new Map(init.map);
        return;
      }
      // some iterable object
      else {
        for (const e in init) {
          this.append(e, init[e]);
        }
      }
      return;
    }
    throw 'Bad constructor arg';
  }

  append(name, value) {
    const lname = name.toLocaleLowerCase();
    const v = this.map.get(lname);
    if (v === undefined) {
      this.map.set(lname, [value]);
    } else {
      v.push(value);
    }
  }
  delete(name) {
    const lname = name.toLocaleLowerCase();
    map[lname] = undefined;
  }
  entries() {
    const foo  = {
      e: this.map.entries()
    }
    foo[Symbol.iterator] = function() {
      return {
        next: function () {
          const ee = foo.e.next();
          if (ee.done) {
            return ee;
          }
          return {
            done: false,
            value: [ee.value[0], ee.value[1].join(', ')]
          };
        }
      };
    };
    return foo;
  }
  forEach(callbackFn, thisArg) {
    if (!thisArg) thisArg = this;
    this.map.forEach(function (value, key, obj) {
      callbackFn.call(thisArg, value.join(', '), key, thisArg);
    });
  }
  get(name) {
    const lname = name.toLocaleLowerCase();
    const v = this.map[lname];
    if (v === undefined) {
      return null;
    }
    return v.join(', '); 
  }
  getSetCookie() {
    const v = map['set-cookie'];
    if (!v) {
      return [];
    }
    return v;
  }
  has(name) {
    return this.map.has(name.toLocaleLowerCase());
  }
  keys() {
    return this.map.keys();
  }
  set(name, value) {
    this.map[name.toLocaleLowerCase()] = value;
  }
  values() {
    const foo  = {
      e: this.map.values()
    }
    foo[Symbol.iterator] = function() {
      return {
        next: function () {
          const ee = foo.e.next();
          if (ee.done) {
            return ee;
          }
          return {
            done: false,
            value: ee.value.join(', ')
          };
        }
      };
    };
    return foo;
  }
};

globalThis.Request = class {
  constructor(input, options) {
    if (!input) throw 'Request() requires an input param';
    // todo
  }
  get body() {

  }
};

globalThis.fetch = function(resource, options) {
  return new Promise(function(resolve, reject) {
      libacorn.fetch(resource, options, resolve, reject);
  });
};


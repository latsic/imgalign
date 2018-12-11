module.exports = {
  "root": true,
  "env": {
    "node": true
  },
  "extends": [
    "plugin:vue/essential",
    "eslint:recommended"
  ],
  "rules": {},
  "parserOptions": {
    "parser": "babel-eslint"
  },
  // extends: [
  //   // add more generic rulesets here, such as:
  //   'eslint:recommended',
  //   'plugin:vue/strongly-recommended'
  // ],
  // rules: {
  //   // override/add rules settings here, such as:
  //   // 'vue/no-unused-vars': 'error'
  // },
  globals: {
    cv: false,
    process: false,
    module: false
  },
  // "parser": "vue-eslint-parser",
  // "parserOptions": {
  //   "parser": "babel-eslint"
  // }
}


// "eslintConfig": {
//   "root": true,
//   "env": {
//     "node": true
//   },
//   "extends": [
//     "plugin:vue/essential",
//     "eslint:recommended"
//   ],
//   "rules": {},
//   "parserOptions": {
//     "parser": "babel-eslint"
//   }
// },
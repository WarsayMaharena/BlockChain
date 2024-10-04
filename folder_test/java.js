const crypto = require('crypto');
const sha256 = data => {
    return crypto
        .createHash('sha256')
        .update(data)
        .digest()
        .toString('hex');
};

console.log(sha256("Hello, World!")); 
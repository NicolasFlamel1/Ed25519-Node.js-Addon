// Header files
#include <new>
#include <node_api.h>
#include <utility>
#include "./Ed25519-WASM-Wrapper-master/main.cpp"

using namespace std;


// Constants

// Operation failed
static napi_value OPERATION_FAILED;


// Function prototypes

// Public key from secret key
static napi_value publicKeyFromSecretKey(napi_env environment, napi_callback_info arguments);

// Sign
static napi_value sign(napi_env environment, napi_callback_info arguments);

// Verify
static napi_value verify(napi_env environment, napi_callback_info arguments);

// Uint8 array to buffer
static pair<const uint8_t *, size_t> uint8ArrayToBuffer(napi_env environment, napi_value uint8Array);

// Buffer to uint8 array
static napi_value bufferToUint8Array(napi_env environment, uint8_t *data, size_t size);

// Boolean to bool
static napi_value booleanToBool(napi_env environment, bool value);


// Main function

// Initialize module
NAPI_MODULE_INIT() {

	// Check if initializing operation failed failed
	if(napi_get_null(env, &OPERATION_FAILED) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}

	// Check if creating public key from secret key property failed
	napi_value temp;
	if(napi_create_function(env, nullptr, 0, publicKeyFromSecretKey, nullptr, &temp) != napi_ok || napi_set_named_property(env, exports, "publicKeyFromSecretKey", temp) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}
	
	// Check if creating sign property failed
	if(napi_create_function(env, nullptr, 0, sign, nullptr, &temp) != napi_ok || napi_set_named_property(env, exports, "sign", temp) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}
	
	// Check if creating verify property failed
	if(napi_create_function(env, nullptr, 0, verify, nullptr, &temp) != napi_ok || napi_set_named_property(env, exports, "verify", temp) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}
	
	// Check if creating operation failed property failed
	if(napi_set_named_property(env, exports, "OPERATION_FAILED", OPERATION_FAILED) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}
	
	// Return exports
	return exports;
}


// Supporting function implementation

// Public key from secret key
napi_value publicKeyFromSecretKey(napi_env environment, napi_callback_info arguments) {

	// Check if no arguments were provided
	size_t argc = 1;
	napi_value argv[argc];
	if(napi_get_cb_info(environment, arguments, &argc, argv, nullptr, nullptr) != napi_ok || argc != sizeof(argv) / sizeof(argv[0])) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting secret key from arguments failed
	const pair<const uint8_t *, size_t> secretKey = uint8ArrayToBuffer(environment, argv[0]);
	if(!secretKey.first) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting public key from secret key failed
	uint8_t publicKey[publicKeySize()];
	if(!publicKeyFromSecretKey(publicKey, secretKey.first, secretKey.second)) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Return public key as a uint8 array
	return bufferToUint8Array(environment, publicKey, sizeof(publicKey));
}

// Sign
napi_value sign(napi_env environment, napi_callback_info arguments) {

	// Check if no arguments were provided
	size_t argc = 2;
	napi_value argv[argc];
	if(napi_get_cb_info(environment, arguments, &argc, argv, nullptr, nullptr) != napi_ok || argc != sizeof(argv) / sizeof(argv[0])) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting message from arguments failed
	const pair<const uint8_t *, size_t> message = uint8ArrayToBuffer(environment, argv[0]);
	if(!message.first) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting secret key from arguments failed
	const pair<const uint8_t *, size_t> secretKey = uint8ArrayToBuffer(environment, argv[1]);
	if(!secretKey.first) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if signing message failed
	uint8_t signature[signatureSize()];
	if(!sign(signature, message.first, message.second, secretKey.first, secretKey.second)) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Return signature as a uint8 array
	return bufferToUint8Array(environment, signature, sizeof(signature));
}

// Verify
napi_value verify(napi_env environment, napi_callback_info arguments) {

	// Check if no arguments were provided
	size_t argc = 3;
	napi_value argv[argc];
	if(napi_get_cb_info(environment, arguments, &argc, argv, nullptr, nullptr) != napi_ok || argc != sizeof(argv) / sizeof(argv[0])) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting message from arguments failed
	const pair<const uint8_t *, size_t> message = uint8ArrayToBuffer(environment, argv[0]);
	if(!message.first) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting signature from arguments failed
	const pair<const uint8_t *, size_t> signature = uint8ArrayToBuffer(environment, argv[1]);
	if(!signature.first) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting public key from arguments failed
	const pair<const uint8_t *, size_t> publicKey = uint8ArrayToBuffer(environment, argv[2]);
	if(!publicKey.first) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if signature failed to verify
	if(!verify(message.first, message.second, signature.first, signature.second, publicKey.first, publicKey.second)) {
	
		// Return false as a bool
		return booleanToBool(environment, false);
	}
	
	// Return true as a bool
	return booleanToBool(environment, true);
}

// Uint8 array to buffer
pair<const uint8_t *, size_t> uint8ArrayToBuffer(napi_env environment, napi_value uint8Array) {

	// Check if uint8 array isn't a typed array
	bool isTypedArray;
	if(napi_is_typedarray(environment, uint8Array, &isTypedArray) != napi_ok || !isTypedArray) {
	
		// Return nothing
		return {nullptr, 0};
	}
	
	// Check if uint8 array isn't a uint8 array
	napi_typedarray_type type;
	size_t size;
	uint8_t *data;
	if(napi_get_typedarray_info(environment, uint8Array, &type, &size, reinterpret_cast<void **>(&data), nullptr, nullptr) != napi_ok || type != napi_uint8_array) {
	
		// Return nothing
		return {nullptr, 0};
	}
	
	// Return data and size
	return {data, size};
}

// Buffer to uint8 array
napi_value bufferToUint8Array(napi_env environment, uint8_t *data, size_t size) {

	// Check if allocating memory for buffer failed
	uint8_t *buffer = new(nothrow) uint8_t[size];
	if(!buffer) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if allocating memory for size hint failed
	size_t *sizeHint = new(nothrow) size_t(size);
	if(!sizeHint) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Copy data
	memcpy(buffer, data, size);
	
	// Clear data
	explicit_bzero(data, size);
	
	// Check if creating array buffer from data failed
	napi_value arrayBuffer;
	if(napi_create_external_arraybuffer(environment, buffer, size, [](napi_env environment, void *finalizeData, void *finalizeHint) {
	
		// Get buffer
		uint8_t *buffer = reinterpret_cast<uint8_t *>(finalizeData);
		
		// Get size hint
		const size_t *sizeHint = static_cast<size_t *>(finalizeHint);
		
		// Clear buffer
		explicit_bzero(buffer, *sizeHint);
		
		// Free memory
		delete [] buffer;
	
	}, sizeHint, &arrayBuffer) != napi_ok) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if creating uint8 array from array buffer failed
	napi_value uint8Array;
	if(napi_create_typedarray(environment, napi_uint8_array, size, arrayBuffer, 0, &uint8Array) != napi_ok) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Return uint8 array
	return uint8Array;
}

// Boolean to bool
napi_value booleanToBool(napi_env environment, bool value) {

	// Check if creating boolean from value failed
	napi_value result;
	if(napi_get_boolean(environment, value, &result) != napi_ok) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Return result
	return result;
}

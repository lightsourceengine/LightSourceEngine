/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

namespace Napi {

template<typename T>
T ObjectGetNumberOrDefault(const Object& object, const char* key, T defaultValue) {
    if (!object.Has(key)) {
        return defaultValue;
    }

    auto value{ object.Get(key) };

    if (!value.IsNumber()) {
        return defaultValue;
    }

    return value.As<Number>();
}

template<typename T>
std::unordered_set<AsyncTask<T>*> AsyncTask<T>::activeTasks;

template<typename T>
AsyncTask<T>::AsyncTask(
    Napi::Env env,
    const std::string& resourceName,
    ExecuteFunction execute,
    CompleteFunction complete)
: env(env), execute(execute), complete(complete) {
    napi_value resourceId;
    napi_status status = napi_create_string_latin1(
        env,
        resourceName.c_str(),
        NAPI_AUTO_LENGTH,
        &resourceId);

    if ((status) != napi_ok) {
        throw Error::New(env);
    }

    status = napi_create_async_work(
        env,
        nullptr,
        resourceId,
        AsyncTask<T>::OnExecute,
        AsyncTask<T>::OnComplete,
        this,
        &this->work);

    if ((status) != napi_ok) {
        throw Error::New(env);
    }

    AsyncTask<T>::activeTasks.insert(this);
    status = napi_queue_async_work(env, this->work);

    if (status != napi_ok) {
        AsyncTask<T>::activeTasks.erase(this);
        napi_delete_async_work(env, this->work);
        this->work = nullptr;
    }

    if ((status) != napi_ok) {
        throw Error::New(env);
    }
}

template<typename T>
AsyncTask<T>::~AsyncTask() {
    AsyncTask<T>::activeTasks.erase(this);

    if (this->work) {
        napi_delete_async_work(this->env, this->work);
        this->work = nullptr;
    }
}

template<typename T>
void AsyncTask<T>::OnExecute(napi_env env, void* self) {
    auto asyncWork{ static_cast<AsyncTask<T>*>(self) };

    if (activeTasks.find(asyncWork) == activeTasks.end()) {
        return;
    }

    try {
        asyncWork->result = asyncWork->execute(asyncWork->env);
        asyncWork->status = napi_ok;
    } catch (std::exception& e) {
        asyncWork->message = e.what();
        asyncWork->status = napi_generic_failure;
    }
}

template<typename T>
void AsyncTask<T>::OnComplete(napi_env env, napi_status status, void* self) {
    auto asyncWork{ static_cast<AsyncTask<T>*>(self) };

    if (activeTasks.find(asyncWork) == activeTasks.end()) {
        return;
    }

    asyncWork->complete(asyncWork->env, asyncWork->result, asyncWork->status, asyncWork->message);
}

} // namespace Napi

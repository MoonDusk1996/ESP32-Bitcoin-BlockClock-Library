#include "blockClockClient.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "WiFiManager.h"
#include "blockClockTypes.h"
#include "blockClockUtils.h"
#include "timeManager.h"

const String MEMPOOL_BASEURL = "https://mempool.space/api";
const String COINLIB_BASEURL = "https://coinlib.io/api/v1";

ApiClient::ApiClient(const String &apiKey) : coinlibApiKey(apiKey) {}

String ApiClient::getBlockHeight() {
  http.begin(MEMPOOL_BASEURL + "/blocks/tip/height");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    return http.getString();
  }
  return "ERR " + httpCode;
}

RecommendedFees ApiClient::getRecommendedFees() {
  RecommendedFees recommendedFees;

  http.begin(MEMPOOL_BASEURL + "/v1/fees/recommended");

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    StaticJsonDocument<192> httpResponseJson;
    String httpResponseBody = http.getString();

    deserializeJson(httpResponseJson, httpResponseBody);

    recommendedFees.high = httpResponseJson["fastestFee"];
    recommendedFees.medium = httpResponseJson["halfHourFee"];
    recommendedFees.low = httpResponseJson["hourFee"];
    recommendedFees.noPriority = httpResponseJson["economyFee"];
    recommendedFees.minimum = httpResponseJson["minimumFee"];
    recommendedFees.error = false;

    return recommendedFees;
  }

  recommendedFees.high = 0;
  recommendedFees.medium = 0;
  recommendedFees.low = 0;
  recommendedFees.noPriority = 0;
  recommendedFees.error = true;

  return recommendedFees;
}

PriceData ApiClient::getBitcoinPrice(CurrencyState currencyState) {
  String currency = currencyStateToString(currencyState);

  DynamicJsonDocument doc(4096);

  const String url = COINLIB_BASEURL + "/coin?key=" + coinlibApiKey +
                     "&pref=" + currency + "&symbol=BTC";

  http.begin(url);

  int httpCode = http.GET();

  PriceData priceData;
  priceData.currency = currencyState;

  if (httpCode == HTTP_CODE_OK) {
    String httpResponseBody = http.getString();
    deserializeJson(doc, httpResponseBody);

    String delta1h = replaceCommaWithDot(doc["delta_1h"]);
    String delta24h = replaceCommaWithDot(doc["delta_24h"]);
    String delta7d = replaceCommaWithDot(doc["delta_7d"]);
    String delta30d = replaceCommaWithDot(doc["delta_30d"]);

    priceData.price = intWithThousandSeparator((int)doc["price"]);
    priceData.change1h = delta1h.toFloat();
    priceData.change24h = delta24h.toFloat();
    priceData.change7d = delta7d.toFloat();
    priceData.change30d = delta30d.toFloat();
    priceData.timestamp = getTimestampFromRTC();
    priceData.error = false;

    return priceData;
  }

  priceData.price = "ERR " + String(httpCode);
  priceData.change1h = (float)0;
  priceData.change24h = (float)0;
  priceData.change7d = (float)0;
  priceData.change30d = (float)0;
  priceData.error = true;

  return priceData;
}

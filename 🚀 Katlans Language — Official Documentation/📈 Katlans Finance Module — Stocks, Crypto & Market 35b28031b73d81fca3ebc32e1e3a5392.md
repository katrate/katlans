# 📈 Katlans Finance Module — Stocks, Crypto & Markets Reference

> Full yfinance + TA-Lib style finance, built into Katlans.
> 

---

## Stock Data

```
~ Get ticker
tk = finticker <"AAPL"> ::
tk = finticker <"RELIANCE.NS"> ::           ~ Indian NSE stocks
tk = finticker <"BTC-USD"> ::              ~ Crypto

~ Real-time price
x = finprice <tk> ::
display x.current ::
display x.open / x.high / x.low / x.close ::
display x.volume / x.prev_close ::
display x.change / x.change_pct ::
display x.bid / x.ask ::
display x.market_cap ::
display x.pe_ratio / x.eps ::
display x.52w_high / x.52w_low ::
display x.avg_volume ::
display x.currency / x.exchange ::
display x.market_state ::                  ~ REGULAR/PRE/POST/CLOSED

~ Historical OHLCV
lt hist = finhist <tk, period<"1y">> ::
lt hist = finhist <tk, start<"2024-01-01">, end<"2025-01-01">> ::
lt hist = finhist <tk, period<"1d">, interval<"5m">> ::
~ Periods: 1d 5d 1mo 3mo 6mo 1y 2y 5y 10y ytd max
~ Intervals: 1m 2m 5m 15m 30m 60m 90m 1h 1d 5d 1wk 1mo 3mo

deta bar belongs in hist |>
    display bar.date ::
    display bar.open / bar.high / bar.low / bar.close ::
    display bar.volume / bar.adj_close ::
<|::

~ Multiple tickers
lt tickers = fintickers <"AAPL", "GOOGL", "MSFT"> ::
ad prices = finprice.multi <tickers> ::
```

---

## Company Info

```
info = fininfo <tk> ::
display info.name / info.sector / info.industry ::
display info.country / info.website / info.description ::
display info.employees / info.ceo / info.founded / info.hq ::
display info.shares_outstanding / info.float_shares ::
display info.institutional_ownership / info.insider_ownership ::
display info.beta / info.forward_pe / info.trailing_pe ::
display info.price_to_book / info.price_to_sales ::
display info.debt_to_equity / info.roe / info.roa ::
display info.profit_margin / info.operating_margin ::
display info.revenue / info.revenue_growth / info.earnings_growth ::
display info.gross_profit / info.ebitda / info.free_cashflow ::
display info.total_cash / info.total_debt ::
display info.dividend_yield / info.dividend_rate / info.payout_ratio ::
display info.target_price / info.recommendation ::
display info.analyst_count ::
```

---

## Technical Indicators

```
~ Moving Averages
lt sma = finind.sma <hist, period<20>> ::
lt ema = finind.ema <hist, period<20>> ::
lt wma = finind.wma <hist, period<20>> ::
lt hma = finind.hma <hist, period<20>> ::
lt vwap = finind.vwap <hist> ::

~ Momentum
lt rsi = finind.rsi <hist, period<14>> ::
lt stoch = finind.stoch <hist, k<14>, d<3>> ::
lt cci = finind.cci <hist, period<20>> ::
lt mfi = finind.mfi <hist, period<14>> ::
lt roc = finind.roc <hist, period<12>> ::
lt williams = finind.williams <hist, period<14>> ::

~ MACD
lt macd, lt signal, lt hist_macd = finind.macd <hist, fast<12>, slow<26>, sig<9>> ::

~ Volatility
lt bb_upper, lt bb_mid, lt bb_lower = finind.bollinger <hist, period<20>, std<2>> ::
lt atr = finind.atr <hist, period<14>> ::
lt keltner_upper, lt keltner_lower = finind.keltner <hist, period<20>> ::
lt stddev = finind.stddev <hist, period<20>> ::

~ Volume
lt obv = finind.obv <hist> ::
lt cmf = finind.cmf <hist, period<20>> ::
lt pvt = finind.pvt <hist> ::

~ Trend
lt adx, lt pdi, lt mdi = finind.adx <hist, period<14>> ::
lt sar = finind.parabolic_sar <hist> ::
lt aroon_up, lt aroon_down = finind.aroon <hist, period<25>> ::
lt ichimoku = finind.ichimoku <hist> ::
display ichimoku.tenkan / ichimoku.kijun / ichimoku.senkou_a / ichimoku.senkou_b ::

~ Support & Resistance
lt levels = finind.support_resistance <hist, method<"pivot">> ::
lt pivots = finind.pivot_points <hist> ::
display pivots.pp / pivots.r1 / pivots.r2 / pivots.r3 ::
display pivots.s1 / pivots.s2 / pivots.s3 ::

~ Candlestick Patterns
x = finpat.doji <hist> ::
x = finpat.hammer <hist> ::
x = finpat.engulfing <hist> ::
x = finpat.morningstar <hist> ::
x = finpat.harami <hist> ::
x = finpat.shooting_star <hist> ::
x = finpat.all <hist> ::                   ~ detect all patterns
```

---

## Charts

```
~ Line chart
finchart.line <hist, field<"close">, title<"AAPL Price">> ::

~ Candlestick
finchart.candle <hist, title<"AAPL">> ::
finchart.candle <hist, volume<true>, indicators<"sma20","bb">> ::

~ OHLC
finchart.ohlc <hist> ::

~ With indicators
chart = finchart.create <hist> ::
finchart.add.sma <chart, period<20>, color<"#0000ff">> ::
finchart.add.ema <chart, period<50>, color<"#ff0000">> ::
finchart.add.bb <chart, period<20>> ::
finchart.add.macd <chart> ::
finchart.add.rsi <chart, period<14>> ::
finchart.add.volume <chart> ::
finchart.show <chart> ::
finchart.save <chart, "chart.png"> ::
finchart.save <chart, "chart.html"> ::     ~ interactive

~ Comparison
finchart.compare <"AAPL", "GOOGL", "MSFT", period<"1y">> ::
```

---

## Options

```
chain = finopt.chain <tk, date<"2025-06-20">> ::
lt dates = finopt.dates <tk> ::

deta call belongs in chain.calls |>
    display call.strike ::
    display call.bid / call.ask ::
    display call.iv ::                     ~ implied volatility
    display call.delta / call.gamma / call.theta / call.vega ::
    display call.volume / call.open_interest ::
    display call.in_the_money ::
<|::

deta put belongs in chain.puts |>
    display put.strike / put.bid / put.ask / put.iv ::
<|::
```

---

## Dividends & Earnings

```
lt div = findiv <tk> ::
deta d belongs in div |>
    display d.date / d.amount / d.type ::
<|::

lt earn = finearn <tk> ::
deta e belongs in earn |>
    display e.date / e.eps_estimate / e.eps_actual ::
    display e.surprise / e.revenue ::
<|::

lt upcoming = finearn.upcoming <days<30>> ::
```

---

## Portfolio Management

```
port = finport.create() ::
finport.add <port, "AAPL", shares<10>, buy_price<150.0>, date<"2024-01-15">> ::
finport.remove <port, "AAPL"> ::
finport.update <port, "AAPL", shares<15>> ::

x = finport.value <port> ::
x = finport.cost <port> ::
x = finport.pnl <port> ::
x = finport.pnl_pct <port> ::
x = finport.today_pnl <port> ::
lt allocation = finport.allocation <port> ::

finport.save <port, "portfolio.json"> ::
port = finport.load <"portfolio.json"> ::

~ Risk metrics
flt sharpe = finport.sharpe <port> ::
flt sortino = finport.sortino <port> ::
flt beta = finport.beta <port> ::
flt alpha = finport.alpha <port> ::
flt maxdraw = finport.max_drawdown <port> ::
flt var = finport.var <port, conf<0.95>> ::

finchart.portfolio <port> ::
```

---

## Crypto & Forex

```
~ Crypto
tk = finticker <"BTC-USD"> ::
lt cryptos = fincrypto.top <n<100>> ::
x = fincrypto.market <tk> ::
display x.market_cap_rank ::
display x.circulating_supply / x.total_supply / x.max_supply ::
display x.ath / x.atl / x.volume_24h ::

~ Forex
x = finforex <"USD", "INR"> ::
display x.rate / x.bid / x.ask ::
lt hist = finforex.hist <"USD", "INR", period<"1y">> ::
lt pairs = finforex.pairs() ::
```

---

## Screener & Market Data

```
~ Screener
lt results = finscreen.run |>
    finscreen.filter <"pe_ratio", "<=", 20> ::
    finscreen.filter <"market_cap", ">=", 1000000000> ::
    finscreen.filter <"sector", "==", "Technology"> ::
    finscreen.sort <"market_cap", "desc"> ::
    finscreen.limit <50> ::
<|::

~ Market Overview
x = finmarket.overview() ::
display x.sp500 / x.nasdaq / x.dow ::
display x.vix / x.advances / x.declines ::
lt sectors = finmarket.sectors() ::
lt movers = finmarket.movers <type<"gainers">, n<10>> ::
lt movers = finmarket.movers <type<"losers">, n<10>> ::
lt movers = finmarket.movers <type<"active">, n<10>> ::

~ Economic Data
x = fineco.gdp <country<"US">> ::
x = fineco.inflation <country<"US">> ::
x = fineco.unemployment <country<"US">> ::
x = fineco.interest_rate <country<"US">> ::
lt calendar = fineco.calendar <days<30>> ::

~ News
lt news = finnews <tk> ::
deta n belongs in news |>
    display n.title / n.url / n.source ::
    display n.date / n.sentiment ::
<|::
lt news = finnews.market() ::

~ Analyst Ratings
lt ratings = finanalyst <tk> ::
deta r belongs in ratings |>
    display r.firm / r.action / r.target / r.date ::
<|::
```

---

## Backtesting

```
bt = finback.create <hist> ::
finback.strategy <bt, fxn myStrategy> ::
finback.cash <bt, 100000> ::
finback.commission <bt, 0.001> ::

fxn dec <myStrategy> <bt, bar> |>
    lt sma = finind.sma <bt.hist, 20> ::
    if bar.close > sma[-1] |>
        finback.buy <bt, shares<10>> ::
    <|::
    elif bar.close < sma[-1] |>
        finback.sell <bt, shares<10>> ::
    <|::
<|::

results = finback.run <bt> ::
display results.final_value ::
display results.total_return ::
display results.sharpe ::
display results.max_drawdown ::
display results.win_rate ::
finchart.backtest <results> ::
```
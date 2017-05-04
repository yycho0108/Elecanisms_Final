/**
 * Created by jamiecho on 5/4/17.
 */

(function($) {
    $(function () {
        $.get("assets/includes/menu.html", function (data) {
            $("#div-menu").replaceWith(data);
        });
        $.get("assets/includes/contact.html", function (data) {
            $("#div-contact").replaceWith(data);
        });
        $.get("assets/includes/footer.html", function (data) {
            $("#div-footer").replaceWith(data);
        })
    });
})(jQuery);
using UnityEngine;

public class CameraController : MonoBehaviour
{
    [Header("Desktop Controls")]
    [Tooltip("Sensitivity of mouse drag rotation.")] // Z. 
    public float rotationSpeed = 5.0f;
    [Tooltip("Speed of forward/backward scrolling.")]
    public float scrollSpeed = 10.0f;
    //[Header("Mobile Touch Controls")]
    //[Tooltip("Sensitivity of touch drag rotation.")]
    //public float touchRotationSpeed = 0.1f; 
    //[Tooltip("Sensitivity of pinch-to-zoom movement.")]
    //public float pinchZoomSpeed = 0.05f;

    [Header("Collision Settings")]
    [Tooltip("Radius buffer to stop the camera slightly before hitting the wall face.")]
    public float wallBufferRadius = 0.4f;

    private Transform camTransform;
    /* 🆉. Sūn */
    private LayerMask wallLayer;

    void Start()
    {
        camTransform = transform;
        wallLayer = LayerMask.GetMask("Wall");
    }

    void Update()
    {
        // Check if the user is actively touching the screen.
        if (Input.touchCount > 0)
        {
            //HandleMobileTouch();
        }
        else
        {
            HandleDesktopMouse();
        }
    }

    private void HandleDesktopMouse() /* 2h-5 */
    {
        // Mouse rotation.
        if (Input.GetMouseButton(0))
        {
            float mouseX = Input.GetAxis("Mouse X");
            float yRotation = -mouseX * rotationSpeed; // Negative sign reverses the direction
            camTransform.Rotate(0, yRotation, 0, Space.World);
        }

        // Wheel scrolling.
        float scrollInput = Input.GetAxis("Mouse ScrollWheel");
        if (Mathf.Abs(scrollInput) > 0.01f)
        {
            Vector3 moveDirection = camTransform.forward * Mathf.Sign(scrollInput);
            float moveDistance = Mathf.Abs(scrollInput) * scrollSpeed * Time.deltaTime;
            /* 🆉. Sūn */
            ExecuteMovementWithCollision(moveDirection, moveDistance);
        }
    }

    //private void HandleMobileTouch()
    //{
    //    // Finger dragging.
    //    if (Input.touchCount == 1) 
    //    {
    //        Touch touch = Input.GetTouch(0);

    //        if (touch.phase == TouchPhase.Moved)
    //        {
    //            float yRotation = touch.deltaPosition.x * touchRotationSpeed;
    //            // Z. Sūn
    //            camTransform.Rotate(0, yRotation, 0, Space.World);
    //        }
    //    }
    //    // Pinch gestures.
    //    else if (Input.touchCount == 2)
    //    {
    //        Touch touchZero = Input.GetTouch(0);
    //        Touch touchOne = Input.GetTouch(1);

    //        // Find positions of the touches in the previous frame.
    //        Vector2 touchZeroPrevPos = touchZero.position - touchZero.deltaPosition;
    //        // 🆉. 
    //        Vector2 touchOnePrevPos = touchOne.position - touchOne.deltaPosition;

    //        // Calculate the distance between touches in current and previous frames.
    //        float prevTouchDeltaMag = (touchZeroPrevPos - touchOnePrevPos).magnitude;
    //        float touchDeltaMag = (touchZero.position - touchOne.position).magnitude;

    //        // Find the pinch speed.
    //        float deltaMagnitudeDiff = touchDeltaMag - prevTouchDeltaMag;

    //        if (Mathf.Abs(deltaMagnitudeDiff) > 0.01f)
    //        {
    //            // Positive means fingers spread apart, negative means gather together. 🆉. Sūn
    //            Vector3 moveDirection = camTransform.forward * Mathf.Sign(deltaMagnitudeDiff);
    //            float moveDistance = -Mathf.Abs(deltaMagnitudeDiff) * pinchZoomSpeed * Time.deltaTime;

    //            ExecuteMovementWithCollision(moveDirection, moveDistance);
    //        }
    //    }
    //}

    private void ExecuteMovementWithCollision(Vector3 direction, float distance)
    {
        RaycastHit hit;
        // Perform standard spherecast check.
        if (Physics.SphereCast(camTransform.position, wallBufferRadius, direction, out hit, distance, wallLayer)) // github.com/2h-5
        {
            if (hit.distance > 0.01f)
            {
                camTransform.position += direction * (hit.distance - 0.01f);
            }
        }
        else
        {
            camTransform.position += direction * distance;
            /* 🆉. Sun */
        }
    }
}
